use std::{
    env::*, 
    os::unix::fs::symlink, 
    path::Path
};

fn symlink_util(exe: &Path, exe_dir: &Path, failed_count: &mut u32, name: &str){
    symlink(exe, exe_dir.join(name)).unwrap_or_else(|e| {
        println!("Error: failed to symlink {}: {}",name,e);
        *failed_count +=1;
    });
}

pub fn run(_args: Vec<String>) {
    let exe = current_exe().expect("Failed to get current executable name");
    let exe_dir = Path::new(&exe).parent().expect("Failed to get executable dir");

    let mut failed_count = 0;
    
    symlink_util(&exe, exe_dir, &mut failed_count, "true");
    symlink_util(&exe, exe_dir, &mut failed_count, "false");
    symlink_util(&exe, exe_dir, &mut failed_count, "ls");
    symlink_util(&exe, exe_dir, &mut failed_count, "whoami");
    symlink_util(&exe, exe_dir, &mut failed_count, "yes");

    if failed_count == 0 {
        println!("All utilities successfully symlinked!");
    } else {
        println!("{} utilities failed to symlink!",failed_count);
    }
}
