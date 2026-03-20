#[derive(Debug ,PartialEq, Eq, Clone)]
enum LexToken {
    Word(String),
    Newline,
    Semicolon,
    Eq,
    And,
    Bg,
    Or,
    EOF
}

#[derive(Debug,PartialEq, Eq)]
pub enum AstNode {
    Sequence(Vec<AstNode>),
    ConstantString(String),
    Command(Box<AstNode>,Vec<AstNode>),
    And(Box<AstNode>,Box<AstNode>),
    Or(Box<AstNode>,Box<AstNode>),
    EnvVarSet(Box<AstNode>,Box<AstNode>),
    ParseEnd,
}

#[derive(PartialEq,Clone, Copy)]
pub enum LexerMode {
    Normal,
    Comment,
    Escape,
    Qoute,
    DQoute
}

fn flush_buf(tokens: &mut Vec<LexToken>, buffer: &mut String) {
    if buffer.len() < 1 {
        return;
    }

    match buffer.as_str() {
        "&&" => tokens.push(LexToken::And),
        "&" => tokens.push(LexToken::Bg),
        "||" => tokens.push(LexToken::Or),
        _    => tokens.push(LexToken::Word(buffer.clone()))
    }

    buffer.clear();
}

fn lex<'a>(cmd: &'a str) -> Vec<LexToken> {
    let mut tokens: Vec<LexToken> = vec![]; 
    let mut buffer = String::new();
    let mut lexer_mode = LexerMode::Normal;
    let mut last_mode = LexerMode::Normal;

    for c in cmd.chars() {
        match lexer_mode {
            LexerMode::Normal => {
                 match c {
                    ' ' => {
                        flush_buf(&mut tokens, &mut buffer); 
                    },
                    '\n' => {
                        flush_buf(&mut tokens, &mut buffer); 
                        //when duplicate newlines fuck with your parser, you just have to get a li'l crafty
                        if tokens.last() != Some(&LexToken::Newline) {
                            tokens.push(LexToken::Newline);
                        }
                    },
                    ';' => {
                        flush_buf(&mut tokens, &mut buffer);
                        tokens.push(LexToken::Semicolon);
                    },
                    '=' => {
                        flush_buf(&mut tokens, &mut buffer);
                        tokens.push(LexToken::Eq);
                    },
                    '#' => {
                        if buffer.len() == 0 {
                            lexer_mode = LexerMode::Comment;
                        } else {
                            buffer.push(c);
                        }
                    },
                    '\'' =>{
                        lexer_mode = LexerMode::Qoute;
                    },
                    '"' =>{
                        lexer_mode = LexerMode::DQoute;
                    },
                    '\\' => {
                        lexer_mode = LexerMode::Escape;
                        last_mode = LexerMode::Normal;
                    },
                    _ => {
                        buffer.push(c);
                    }
                }       
            }, 
            LexerMode::Comment => {
                match c {
                    '\n' => {
                        lexer_mode = LexerMode::Normal;
                        tokens.push(LexToken::Newline);
                    },
                    
                    _ => ()
                }
            },
            LexerMode::Qoute => {
                match c {
                    '\'' =>{
                        lexer_mode = LexerMode::Normal;
                    },
                    '\\' => {
                        lexer_mode = LexerMode::Escape;
                        last_mode = LexerMode::Qoute;
                    },
                    _ => {
                        buffer.push(c);
                    }
                }
            },
            LexerMode::DQoute => {
                match c {
                    '"' =>{
                        lexer_mode = LexerMode::Normal;
                    },
                    '\\' => {
                        lexer_mode = LexerMode::Escape;
                        last_mode = LexerMode::DQoute;
                    },
                    _ => {
                        buffer.push(c);
                    }
                }
            },
            LexerMode::Escape => {
                if last_mode == LexerMode::Normal {
                    buffer.push(c);
                } else {
                    match c {
                        '\\' => buffer.push('\\'),
                        'a' => buffer.push('\x07'),
                        'b' => buffer.push('\x09'),
                        'e' => buffer.push('\x1B'),
                        'f' => buffer.push('\x0C'),
                        'n' => buffer.push('\n'),
                        'r' => buffer.push('\r'),
                        't' => buffer.push('\t'),
                        'v' => buffer.push('\x0B'),
                        _ => {
                            buffer.push('\\');
                            buffer.push(c)
                        }, 
                    }        
                }
                
                lexer_mode = last_mode;
            }
        }
    }

    if buffer.len() > 0 {
        flush_buf(&mut tokens, &mut buffer);
    }

    return tokens;
}

struct Peeker {
    tokens: Vec<LexToken>
}

impl Peeker {
    fn new(tokens: Vec<LexToken>) -> Self {
        Self { tokens }
    }

    fn peek(&self,i: usize) -> LexToken {
        if i >= self.tokens.len() {
            return LexToken::EOF;
        }
        self.tokens[i].clone()
    }
    
    fn consume(&mut self) -> LexToken {
        if 1 > self.tokens.len() {
            return LexToken::EOF;
        }
        self.tokens.remove(0)
    }

}

fn parse_expr(peeker: &mut Peeker) -> AstNode{
    if let LexToken::Word(w) = peeker.peek(0) {
        peeker.consume(); // only consume it if it matches 
        AstNode::ConstantString(w) 
    } else {
        AstNode::ParseEnd 
    }
}

fn parse_command (peeker: &mut Peeker) -> AstNode {
    let command = parse_expr(peeker);
    
    if command == AstNode::ParseEnd {
        return AstNode::ParseEnd;
    }
    
    if peeker.peek(0) == LexToken::Eq{
        peeker.consume();
        let val = parse_expr(peeker);

        return AstNode::Sequence(vec![
            AstNode::EnvVarSet(Box::new(command), Box::new(val)),
            parse_command(peeker)
        ])
    }
    
    let mut args: Vec<AstNode> = vec![];

    loop {
        let expr = parse_expr(peeker);

        if expr == AstNode::ParseEnd {
            break;
        }

        args.push(expr);
    };

    AstNode::Command(Box::new(command), args)
}

fn parse_command_expr (peeker: &mut Peeker) -> AstNode{
    let mut expr = AstNode::ParseEnd;
    loop {
        let next = peeker.peek(0);
        if next == LexToken::And {
            peeker.consume();
            let command = parse_command(peeker);
            expr = AstNode::And(Box::new(expr), Box::new(command)); 
        } else if next == LexToken::Or {
            peeker.consume();
            let command = parse_command(peeker);
            expr = AstNode::Or(Box::new(expr), Box::new(command)); 
        } else if next == LexToken::Semicolon || next == LexToken::Newline|| next == LexToken::EOF {
            return expr;
        } else {
            
            let command = parse_command(peeker);  
            if command == AstNode::ParseEnd {
                return expr;
            } else {
                expr = command;
            }
        }
    }    
}

fn parse_sequence(mut peeker: Peeker) -> AstNode{
    let mut nodes = Vec::<AstNode>::new();
   
    loop {
        let cmdexpr = parse_command_expr(&mut peeker);

        if cmdexpr == AstNode::ParseEnd {
            break;
        } else {
            let next = peeker.peek(0); 

            if next == LexToken::Semicolon || next == LexToken::Newline {
                nodes.push(cmdexpr);
                peeker.consume();
            } else if next == LexToken::EOF {
                break;
            }
        }
    }

    return AstNode::Sequence(nodes);
}

pub fn parse<'a>(cmd: &'a str) -> AstNode {
    let lexed = lex(cmd);
    println!("{lexed:?}");
    let peeker = Peeker::new(lexed);

    return parse_sequence(peeker);
}
