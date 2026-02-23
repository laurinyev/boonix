use std::{
    env::*,
};

mod parser {
    #[derive(Debug ,PartialEq, Eq)]
    enum LexToken<'a>{
        Word(&'a str),
        Newline,
        Semicolon
    }

    #[derive(Debug,PartialEq, Eq)]
    pub enum AstNode<'a> {
        Root(Vec<AstNode<'a>>),
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

        //println!("{:?}",lexed);
        
        loop {
            let node = parse_expr(&mut lexed,false);
            if node == AstNode::ParseEnd {
                break;
            }
            if node != AstNode::DiscardMe {
                nodes.push(node);
            }
        }

        return AstNode::Root(nodes);
    }
}

mod interp {
    use {
        std::{
            env::*,
            process::*
        },
        crate::parser::*
    };

    fn interp_ast<'a>(node: AstNode<'a>, is_nested: bool) -> String{
        match node {
            AstNode::Root(_) => "".to_string(),
            AstNode::ParseEnd => "".to_string(),
            AstNode::DiscardMe => "".to_string(),
            AstNode::ExternalCommand(_,_) => "".to_string(),
            AstNode::BuiltinCd(n) => {
                let mut dir = interp_ast(*n,true);
                if is_nested {
                    return "".to_string();
                }
                if dir == "" {
                    match var("HOME") {
                        Ok(v) => {
                            dir = v;
                        },
                        Err(..) => {
                            eprintln!("cd: HOME isn't set") 
                        }
                    } 
                }
                match set_current_dir(dir) {
                    Ok(..) => unsafe {
                        set_var("PWD",current_dir().unwrap());
                    },
                    Err(e) => {
                        eprintln!("Couldn't change directory: {e}");
                    }
                };
                return "".to_string();
            },
            AstNode::ConstantString(v) => v.to_string(),
            AstNode::BuiltinExit => {
                if !is_nested {
                    exit(0)
                } else {
                    "".to_string()
                } 
            },
        }
    }

    pub fn run(cmd: &str) -> u32 {
        let ast = parse(cmd);
       
        println!("{:?}",ast);

        if let AstNode::Root(v) = ast{
            if v.len() == 0 {
                return 1;
            }

            for c in v {
                _ = interp_ast(c,false);
            }
        } else {
            panic!("parse() returned a none-root AST node");
        }

        return 0;
    }    
}

mod repl {
    use {
        std::{
            env::*, 
            io::*
        },
        super::interp
    };

    fn print_prompt() {
       match var("PS1") {
            Ok(v) => {
                print!("{}",v);
            },
            Err(..) => {
                match var("PWD") {
                    Ok(v) => {
                        print!("{}> ",v);
                    },
                    Err(..) => {
                        print!("> ");
                    }
                } 
            }
        } 
        stdout().flush().unwrap();
    }
    
    pub fn run() {
        loop {
            print_prompt();
            let mut instr = String::new(); 
            stdin().read_line(&mut instr).unwrap();
            interp::run(instr.as_str());
        }
    }
}


fn main() {
    let mut args = args();

    if args.len() < 2 {
        repl::run();
    } else if args.len() < 4 {
        let second_arg  = args.nth(1).unwrap();
        if second_arg == "--help" {
            println!("just run the shell lmao, not like I have any options xD");
        } else {
            println!("script: {}", second_arg);
        }
    }
}
