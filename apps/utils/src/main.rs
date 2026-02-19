use {
    std::{
        env::*,
        path::*,
        process::*
    }
};

fn help() {
    println!("This is help");
}

fn invoke_util(args: Vec<String>){
    match args[0].as_str() {
        "true" => {
            exit(0);
        },
        "false" => {
            exit(1);
        },
        _ => {
            help();
        }
    } 
}
fn main() {
    let mut args: Vec<String> = args().collect();

    if args.len() == 1 {
        args[0] = String::from(Path::new(&args[0]).file_name().unwrap().to_str().unwrap());
    } else {
        _ = args.remove(0);
    }
    invoke_util(args);
}

