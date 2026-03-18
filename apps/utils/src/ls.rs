use {
    std::{
        fs::*, os::unix::fs::{FileTypeExt, PermissionsExt}, path::*, process::*
    }
};

pub fn run(args: Vec<String>){
    //TODO: all the fucking options
    let colors = true;

    let default = Path::new(".");
    let dir = args.get(1).map(|s| Path::new(s)).unwrap_or(&default);

    let mut entries = match read_dir(dir) {
        Ok(v) => v,
        Err(e) => {
            println!("Couldn't list directory: {e}");
            exit(1);
        }
    }
    .filter_map(|a| a.ok())
    .collect::<Vec<_>>();

    entries.sort_by_key(|a| a.file_name());

    for e in entries {
        let e_type = e.file_type().expect("Failed to get file type");
        //TODO: parse $LS_COLORS
        if colors {
            if e_type.is_dir() {
                print!("\x1B[01;34m");
            } else if e_type.is_symlink() {
                print!("\x1B[01;36m");
            } else if e_type.is_socket() {
                print!("\x1B[01;35m");
            } else if e_type.is_file() && e.metadata().expect("Failed to get perms").permissions().mode() & 0o111 != 0 {
                print!("\x1B[01;32m");
            }
        }
        
        print!("{}  ",e.file_name().display());
        if colors && !e_type.is_file(){
            print!("\x1B[01m"); 
        } 
    }
    print!("\n"); 
}
