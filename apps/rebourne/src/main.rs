use std::{
    env::*,
};

mod repl {
    use std::{
        env::*, io::*
    };

    fn print_prompt() {
       match var("PS1") {
            Ok(v) => {
                print!("{} ",v);
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
            print!("{}",instr); 
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
