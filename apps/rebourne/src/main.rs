use std::{
    env::*,
};

mod interp {
    #[derive(Debug)]
    #[allow(dead_code)]
    enum LexToken<'a>{
        Word(&'a str)
    }

    fn lex<'a>(cmd: &'a str) -> Vec<LexToken<'a>> {
        let mut tokens: Vec<LexToken> = vec![]; 
        let mut word_start_idx = None;

        for (i,c) in cmd.chars().enumerate() {
            if word_start_idx.is_some() && c.is_whitespace() {
                tokens.push(LexToken::Word(&cmd[word_start_idx.unwrap()..i])); 
                word_start_idx = None;
            }

            if c.is_alphabetic() && word_start_idx.is_none() {
                word_start_idx = Some(i);
            } 
        };
        return tokens;
    }

    pub fn run(cmd: &str) -> u32 {
        println!("{:?}",lex(cmd));

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
                        print!("{}>",v);
                    },
                    Err(..) => {
                        print!(">");
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
        println!("script: {}", args.nth(1).unwrap());
    }
}
