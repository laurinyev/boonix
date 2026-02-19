use {
    std::{
        fs::*,
        path::*,
        process::*
    }
};

pub fn run(args: Vec<String>){
    //TODO: all the fucking options
    let default = String::from(".");
    let dir = args.iter().nth(1).unwrap_or(&default);

    let mut entries : Vec<_> = match read_dir(Path::new(dir)) {
        Ok(v) => v,
        Err(e) => {
            println!("Couldn't list directory: {e}");
            exit(1);
        }
    }.map(|e| {
        e.unwrap().file_name().to_str().unwrap().to_string()   
    })
    .collect();

    entries.sort();

    for e in entries {
        print!("{}  ",e);
    }
    print!("\n"); 
}
