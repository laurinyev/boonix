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
    BuiltinCd(Box<AstNode>),
    BuiltinExit,
    ExternalCommand(String,Vec<AstNode>),
    ParseEnd,
    DiscardMe
}

fn lex<'a>(cmd: &'a str) -> Vec<LexToken> {
    let mut tokens: Vec<LexToken> = vec![]; 
    let mut buffer = String::new();

    for c in cmd.chars() {
        match c {
            ' ' => {
                tokens.push(LexToken::Word(buffer.clone()));
                buffer.clear();
            },
            '\n' => {

            },
            _ => {
                buffer.insert(buffer.len(), c);
            }

        } 
    }

    if buffer.len() > 0 {
        tokens.push(LexToken::Word(buffer.clone()));
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
        if i > self.tokens.len() {
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

fn parse_expr(mut peeker: Peeker) -> AstNode{
    if let LexToken::Word(w) = peeker.next() {
        AstNode::ConstantString(w) 
    } else {
        AstNode::ParseEnd 
    }
}

fn parse_command (mut peeker: Peeker) -> AstNode{
    if let LexToken::Word(w) = peeker.next() {
        match w.as_str() {
            "exit" => {
                AstNode::BuiltinExit
            },
            "cd" => {
                AstNode::BuiltinCd(Box::new(parse_expr(peeker)))
            },
            _ => {
                AstNode::ParseEnd
            }
        }
    } else {
        AstNode::ParseEnd
    }

}

fn parse_sequence(peeker: Peeker) -> AstNode{
    let mut nodes = Vec::<AstNode>::new();
    
    nodes.push(parse_command(peeker));

    return AstNode::Sequence(nodes);
}

pub fn parse<'a>(cmd: &'a str) -> AstNode {
    let lexed = lex(cmd);
    let peeker = Peeker::new(lexed);

    return parse_sequence(peeker);
}
