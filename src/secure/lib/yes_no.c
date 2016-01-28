/// @file yes_no.c
/// @brief library module for yes/no questions
/// @author Gwenhwyvar
/// @version 0.0.0
/// @date 2016-01-28

private void yes_no(string prompt, mixed call_back, string default = "n");

protected void yes_no_handler(string input, string prompt, mixed call_back, string default)
{
    int i;

    if(input == "")
        input = default;
    if((input == "y") || (input == "Y"))
        i = 0;
    else if((input == "n") || (input == "N"))
        i = 1;
    else
    {
        yes_no(prompt, call_back, default);
        return;
    }
    (*call_back[i])(call_back[2..]);
}

private void yes_no(string prompt, mixed call_back, string default = "n")
{
    receive(promtp);
    get_char((: yes_no_handler :), I_NONE, prompt, call_back, default);
}
