const COW_STRING: &str = r#"
\   ^__^
 \  (oo)\_______
    (__)\       )\/\
        ||----w |
        ||     ||"#;

pub fn run(args: Vec<String>) {
    let string = format!(" {} ",args[1..].join(" "));

    let bubble = format!(" {nothing:_<width$}\n<{string}>\n {nothing:-<width$}",
                        nothing = "",
                        width = string.len());

    let pad = format!("{: <w$}","",w = string.len() + 2);
    
    let cow = COW_STRING.to_string().replace("\n", &format!("\n{pad}"));

    println!("{bubble}{cow}")
}
