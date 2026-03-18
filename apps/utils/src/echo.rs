use std::process::exit;

#[derive(PartialEq,)]
enum EscapeInterpMode {
    Normal,
    Escape,
    Hex,
    Oct
}

pub fn run(args: Vec<String>) {
    let count = args.len();
    let mut last_newline = true;
    let mut interp_escapes = false;
    let mut esc_interp_mode = EscapeInterpMode::Normal;
    let mut chars_remaining = 0; // characters remaining from a literal(\x or \0)
    let mut num_buf = String::with_capacity(3);

    for (i, arg) in args.into_iter().skip(1).enumerate() {
        match arg.as_str() {
            "-n" => last_newline = false,
            "-e" => interp_escapes = true,
            "-E" => interp_escapes = false,
            _ => {
                if interp_escapes {
                    for c in arg.chars() {
                        match esc_interp_mode {
                            EscapeInterpMode::Normal => {
                                if c == '\\' {
                                   esc_interp_mode = EscapeInterpMode::Escape; 
                                } else {
                                    print!("{c}");
                                }     
                            },
                            EscapeInterpMode::Escape => {
                                match c {
                                    '\\' => print!("\\"),
                                    'a' => print!("\x07"),
                                    'b' => print!("\x08"),
                                    'c' => exit(1),
                                    'e' => print!("\x1B"),
                                    'f' => print!("\x0C"),
                                    'n' => print!("\n"),
                                    'r' => print!("\r"),
                                    't' => print!("\t"),
                                    'v' => print!("\x0B"),
                                    'x' => {
                                        chars_remaining = 2;
                                        esc_interp_mode = EscapeInterpMode::Hex;
                                    },
                                    '0' => {
                                        chars_remaining = 3;
                                        esc_interp_mode = EscapeInterpMode::Oct;
                                    }
                                    _ => print!("\\{c}"),
                                }
                                //Don't modify it if it changed
                                if esc_interp_mode == EscapeInterpMode::Escape {
                                    esc_interp_mode = EscapeInterpMode::Normal;
                                }
                            },
                            EscapeInterpMode::Hex => {
                                if chars_remaining == 0 || !c.is_digit(16) {
                                    let new_c = u8::from_str_radix(&num_buf, 16).expect("failed to convert characters into number!");
                                    print!("{}", new_c as char);
                                    num_buf.clear();
                                    esc_interp_mode = EscapeInterpMode::Normal;
                                } else {
                                    num_buf.push(c);
                                    chars_remaining -= 1;
                                }
                            },
                            EscapeInterpMode::Oct => {
                                if chars_remaining == 0 || !c.is_digit(8) {
                                    let new_c = u8::from_str_radix(&num_buf, 8).expect("failed to convert characters into number!");
                                    print!("{}", new_c as char); 
                                    num_buf.clear();
                                    esc_interp_mode = EscapeInterpMode::Normal;
                                } else {
                                    num_buf.push(c);
                                    chars_remaining -= 1;
                                }
                            }
                        }
                    }
                    if esc_interp_mode == EscapeInterpMode::Hex{
                        let new_c = u8::from_str_radix(&num_buf, 16).expect("failed to convert characters into number!");
                        print!("{}", new_c as char); 
                        num_buf.clear();
                        esc_interp_mode = EscapeInterpMode::Normal;
                    } else if esc_interp_mode == EscapeInterpMode::Oct{
                        let new_c = u8::from_str_radix(&num_buf, 8).expect("failed to convert characters into number!");
                        print!("{}", new_c as char); 
                        num_buf.clear();
                        esc_interp_mode = EscapeInterpMode::Normal;
                    }
                } else {
                    print!("{arg}");
                    if i < count - 1 {
                        print!(" ");
                    }
                }
            }
        };
        
        if last_newline {
            println!();
        }
    }
}
