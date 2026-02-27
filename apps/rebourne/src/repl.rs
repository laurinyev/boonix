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
