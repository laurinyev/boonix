use {
    crate::{parser::*, resolver::resolve}, std::{
        env::*,
        process::*
    }
};

fn interp_ast<'a>(node: &AstNode<'a>, is_nested: bool) -> String{
    match node {
        AstNode::Sequence(nodes) => {
            let mut toret = String::new();
            for n in nodes {
                toret += &interp_ast(n, is_nested)
            }
            toret
        },
        AstNode::ParseEnd => "".to_string(),
        AstNode::DiscardMe => "".to_string(),
        AstNode::ExternalCommand(cmd,_args) => {
            let util_path = resolve(cmd); 
            if util_path.is_none() {
                return "".to_string() 
            }
            let args = _args.iter().map(|a| interp_ast(a, true));
            let mut child = Command::new(util_path.unwrap());

            child.args(args);
            if !is_nested {
                child.stdin(Stdio::inherit());
                child.stdout(Stdio::inherit());
                child.stderr(Stdio::inherit());
            }

            let out = child.output(); 
            if is_nested {
                match out {
                    Ok(o) => match String::from_utf8(o.stdout) {
                            Ok(s) => s,
                            Err(_) => "".to_string(),
                    },
                    Err(_) => "".to_string(), 
                }
            } else {
                "".to_string() 
            }
            
        },
        AstNode::BuiltinCd(n) => {
            let mut dir = interp_ast(n,true);
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
    let root = parse(cmd);
    println!("{:?}",root);
    interp_ast(&root,false);
    
    return 0;
}
