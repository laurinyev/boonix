#[derive(Debug ,PartialEq, Eq)]
enum LexToken<'a>{
    Word(&'a str),
    Newline,
    Semicolon
}

#[derive(Debug,PartialEq, Eq)]
pub enum AstNode<'a> {
    Sequence(Vec<AstNode<'a>>),
    ConstantString(&'a str),
    BuiltinCd(Box<AstNode<'a>>),
    BuiltinExit,
    ExternalCommand(&'a str,Vec<AstNode<'a>>),
    ParseEnd,
    DiscardMe
}

fn lex<'a>(cmd: &'a str) -> Vec<LexToken<'a>> {
    let mut tokens: Vec<LexToken> = vec![]; 
    let mut word_start_idx = None;

    for (i,c) in cmd.chars().enumerate() {
        if word_start_idx.is_some() && (c.is_whitespace() || c == ';') {
            tokens.push(LexToken::Word(&cmd[word_start_idx.unwrap()..i])); 
            word_start_idx = None;
        }
        
        if c == '\n' {
            tokens.push(LexToken::Newline)
        } else if c == ';' {
            tokens.push(LexToken::Semicolon)
        } else if !c.is_whitespace() && word_start_idx.is_none() {
            word_start_idx = Some(i);
        }
    };

    if word_start_idx.is_some() {
        tokens.push(LexToken::Word(&cmd[word_start_idx.unwrap()..])); 
    }
    return tokens;
}

fn parse_expr<'a>(lexed: &mut Vec<LexToken<'a>>, is_args: bool ) -> AstNode<'a>{
    match lexed[..] {
        [LexToken::Word("cd"),..] => {
            lexed.remove(0);
            
            let next = parse_expr(lexed,true);

            AstNode::BuiltinCd(Box::new(next))
        },
        [LexToken::Word("exit"),..] => {
            lexed.remove(0);
            AstNode::BuiltinExit
        },
        [LexToken::Word(w),..] if is_args => {
            lexed.remove(0);
            AstNode::ConstantString(w)
        },
        [LexToken::Word(w),..] if !is_args => {
            lexed.remove(0);
           
            let mut nodes = vec![];
            loop {
                let next = parse_expr(lexed,true);
                if next == AstNode::DiscardMe {
                    break;
                }
                nodes.push(next);
            }
            
            AstNode::ExternalCommand(w,nodes)
        },
        [LexToken::Semicolon,..] | [LexToken::Newline,..] => {
            lexed.remove(0);
            AstNode::DiscardMe
        },
        _ => {
            AstNode::ParseEnd
        }
    }
}

pub fn parse<'a>(cmd: &'a str) -> AstNode<'a> {
    let mut nodes = vec![];
    let mut lexed = lex(cmd);

    println!("{:?}",lexed);
    
    loop {
        let node = parse_expr(&mut lexed,false);
        if node == AstNode::ParseEnd {
            break;
        }
        if node != AstNode::DiscardMe {
            nodes.push(node);
        }
    }

    return AstNode::Sequence(nodes);
}
