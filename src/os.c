#include "os.h"

void initOS(OS_t* os)
{
    local_os = (OS_t*)malloc(sizeof(OS_t));
    memset(local_os->input,'\0',MAX_INPUT);
    local_os->input_length = 0;
    flashOS(local_os, NULL, 0);
    local_os->line_length = 0;
    local_os->grabbed = false;
    local_os->isskip = false;
    local_os->ostime = 0.f;
}

void freeOS(OS_t* os)
{
    free(local_os);
}

bool commandOS(OS_t* os, char* command)
{
    if (!strcmp(command, "server")) {
        if (startServerNetwork(7373)) {
            pushlineOS(os, "FAILED TO CREATE SERVER");
        } else {
            pushlineOS(os, "STARTED SERVER SUCCESSFULY");
        }
        return true;
    }
    else if (!strcmp(command, "client")) {
        if (startClientNetwork("192.168.1.203", 7373)) {
            pushlineOS(os, "FAILED TO START CLIENT");
        } else {
            pushlineOS(os, "CREATED CLIENT SUCCESSFULY");
        }
        return true;
    }
    else if (!strcmp(command, "test")) {
        for (int i = 0; i < 20; i++) {
            // pushlineOS(os, "test line");
            pushlineOS(os, FormatText("test line %d", i));
        }
        return true;
    }
    else if (!strcmp(command, "clear")) {
        flashOS(local_os, NULL, 0);
    }
    else if (!strcmp(command, "fuck")) {
        pushlineOS(local_os, "I just did sex so hard to this girl. She was moaning and stuff and i did sex so hard and so good with her. She was all \"Please have good sex with me\" and I said back \"Yeah, I'm going to really sex you so great lady\" and she's like \"Thanks\". It was so good and so hot and we both loved making a sex on all the positions. At the end I was all \"Hey, I'm going to finish sex now\" and she said \"Yes, that would be nice if you finished so hard on that towel\". I said \"Yes\" and I finished so hard on the towel. And It was so hot.");
    }
    else if (!strcmp(command, "buffer")) {
        for (int i = 0; i < os->line_length; i++) {
            printf("%s\n", os->lines[i]);
        }
    }
    else {
        pushlineOS(local_os, "this string is way too long to fit in the max input size of the command line because im a terrible programmer and don't know what the fuck im doing");
    }
    return false;
}

void updateOS(OS_t* os)
{
    os->ostime += GetFrameTime()*2;
    
    if (!os->grabbed) {
        if (IsKeyPressed(KEY_E)) {
            os->grabbed = true;
            os->isskip = false;
        }
        return;
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        os->grabbed = false;
        return;
    }
    if (!os->isskip) {
        os->isskip = true;
        return;
    }
    
    
    int k = GetKeyPressed();
    bool k_is_allowed = false;
    for (int i = 0; i < strlen(clist); i++) {
        if (clist[i] == k) {
            k_is_allowed = true;
            break;
        }
    }
    if (k_is_allowed) {
        if (os->input_length < MAX_INPUT) {
            os->input[os->input_length] = (char)k;
            os->input_length++;
        }
    } else if (k == 259) {
        if (os->input_length > 0) {
            os->input[os->input_length-1] = 0;
            os->input[os->input_length] = '\0';
            os->input_length--;
        }
    } else if (k == 32) {
        if (os->input_length < 52) {
            os->input[os->input_length] = ' ';
            os->input_length++;
        }
    } else if (k == KEY_ENTER) {
        commandOS(os, os->input);
        memset(os->input,0,strlen(os->input));
        os->input_length = 0;
    }
    else if (k > -1) {
        printf("%d\n", k);
    }
}

void flashOS(OS_t* os, char lines[MAX_LINES][MAX_INPUT], int num_lines)
{
    if (lines == NULL) {
        for (int i = 0; i < MAX_LINES; i++) {
            pushlineOS(os, "");
        }
        os->line_length = 0;
        return;
    }
    os->line_length = 0;
    for (int i = 0; i < num_lines; i++) {
        pushlineOS(os, lines[i]);
    }
    
}

void pushlineOS(OS_t* os, const char* line)
{
    if (strlen(line) > MAX_INPUT) {
        int n = 0;
        while (n < strlen(line)) {
            char str[MAX_INPUT];
            for (int i = 0; i < MAX_INPUT-1; i++) {
                str[i] = line[n+i];
            }
            str[MAX_INPUT] = '\0';
            pushlineOS(os, str);
            n+=MAX_INPUT-1;
        }
        return;
    }
    if (os->line_length == MAX_LINES) {
        for (int i = 0; i < MAX_LINES-1; i++) {
            strcpy(os->lines[i], os->lines[i+1]);
        }
    } else {
        os->line_length++;
    }
    strcpy(os->lines[os->line_length-1], line);
}

void drawOS(OS_t* os, Screen_t* scr)
{
    DrawRectangle(0, 0, scr->texture.texture.width*screen_w_gl, scr->texture.texture.height*screen_h_gl, BLACK);
    for (int i = 0; i < MAX_LINES; i++) {
        DrawText(FormatText("%s", os->lines[i]), 5, 5+i*22, 20, GREEN);
    }
    int iw = MeasureText(FormatText(">%s", os->input), 20);
    DrawText(FormatText(">%s", os->input), 5, 5+os->line_length*22, 20, GREEN);
    if ((int)os->ostime%2==1) {
        DrawRectangle(5+iw+1, 5+os->line_length*22, 10, 20, GREEN);
    }
}
