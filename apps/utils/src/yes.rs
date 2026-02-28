pub fn run(args: Vec<String>) {
    let mut string = "y";
    
    if args.len() > 1 {
        string = &args[1];
    }

    loop {
        println!("{}",string);
    }
}
