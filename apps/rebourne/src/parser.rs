#[derive(Debug ,PartialEq, Eq, Clone)]
enum LexToken {
    Word(String),
    Newline,
    Semicolon,
    And,
    Or,
    EOF
}

#[derive(Debug,PartialEq, Eq)]
pub enum AstNode {
    Sequence(Vec<AstNode>),
    ConstantString(String),
    Command(Box<AstNode>,Vec<AstNode>),
    ParseEnd,
}

fn flush_buf(tokens: &mut Vec<LexToken>, buffer: &mut String) {
    if buffer.len() < 1 {
        return;
    }

    match buffer.as_str() {
        "&&" => tokens.push(LexToken::And),
        "||" => tokens.push(LexToken::Or),
        _    => tokens.push(LexToken::Word(buffer.clone()))
    }

    buffer.clear();
}

fn lex<'a>(cmd: &'a str) -> Vec<LexToken> {
    let mut tokens: Vec<LexToken> = vec![]; 
    let mut buffer = String::new();

    for c in cmd.chars() {
        match c {
            ' ' => {
                flush_buf(&mut tokens, &mut buffer); 
            },
            '\n' => {
                flush_buf(&mut tokens, &mut buffer); 
                tokens.push(LexToken::Newline);
            },
            ';' => {
                flush_buf(&mut tokens, &mut buffer);
                tokens.push(LexToken::Semicolon);
            },
            _ => {
                buffer.insert(buffer.len(), c);
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
    
    fn next(&mut self) -> LexToken {
        if 1 > self.tokens.len() {
            return LexToken::EOF;
        }
        self.tokens.remove(0)
    }

}

fn parse_expr(peeker: &mut Peeker) -> AstNode{
    if let LexToken::Word(w) = peeker.peek(0) {
        peeker.next(); // only consume it if it matches 
        AstNode::ConstantString(w) 
    } else {
        AstNode::ParseEnd 
    }
}

fn parse_command (peeker: &mut Peeker) -> AstNode{
    let command = parse_expr(peeker);
    if command == AstNode::ParseEnd {
        return AstNode::ParseEnd;
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

fn parse_sequence(mut peeker: Peeker) -> AstNode{
    let mut nodes = Vec::<AstNode>::new();
   
    loop {
        let cmd = parse_command(&mut peeker);
        
        if let AstNode::Command(..) = &cmd {
            let next = peeker.peek(0); 

            if next == LexToken::Semicolon || next == LexToken::Newline {
                nodes.push(cmd);
                peeker.next();
            } else if next == LexToken::EOF {
                break;
            }
        } else {
            break;
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
