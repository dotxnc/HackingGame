#include "os.h"

void initOS(OS_t* os)
{
    // local_os = (OS_t*)malloc(sizeof(OS_t));
    
    local_os.input_length = 0;
    local_os.line_length = 0;
    local_os.grabbed = false;
    local_os.isskip = false;
    local_os.ostime = 0.f;
    local_os.online = false;
    local_os.program = OFFLINE;
    local_os.chat_length = 0;
    local_os.bootline = 0;
    local_os.bootline_timeout = 0.f;
    local_os.username_length = 0;
    
    flashOS(&local_os, NULL, 0);
    memset(local_os.input,'\0',MAX_INPUT);
    memset(local_os.username,'\0',MAX_USERNAME);
    for (int i = 0; i < MAX_LINES; i++)
        memset(local_os.chatlog[i], '\0', MAX_INPUT);
}

void freeOS(OS_t* os)
{
    // free(local_os);
}

bool commandOS(OS_t* os, char* command)
{
    
    char args[MAX_ARGS][MAX_INPUT];
    int argc = 0;
    char* pch;
    pch = strtok(command, " ");
    int i=0;
    while (pch != NULL) {
        if (i<MAX_ARGS) {
            strcpy(args[i], pch);
            pch = strtok(NULL, " ");
            i++;
            argc++;
        } else {
            pch=strtok(args[i], "");
        }
    }
    free(pch);
    
    if (!strcmp(args[0], "server")) {
        int port = atoi(args[1]);
        if (argc < 2) {
            port = DEFAULT_PORT;
        }
        if (startServerNetwork(port)) {
            pushlineOS(os, "FAILED TO CREATE SERVER");
        } else {
            pushlineOS(os, "STARTED SERVER SUCCESSFULY");
        }
        return true;
    }
    else if (!strcmp(args[0], "client")) {
        int port = atoi(args[2]);
        char* ip = args[1];
        if (argc < 3) {
            port = DEFAULT_PORT;
            ip = DEFAULT_IP;
        }
        if (startClientNetwork(ip, port)) {
            pushlineOS(os, "FAILED TO START CLIENT");
        } else {
            pushlineOS(os, "CREATED CLIENT SUCCESSFULY");
        }
        return true;
    }
    else if (!strcmp(args[0], "test")) {
        for (int i = 0; i < 20; i++) {
            // pushlineOS(os, "test line");
            pushlineOS(os, FormatText("test line %d", i));
        }
        return true;
    }
    else if (!strcmp(args[0], "clear")) {
        flashOS(&local_os, NULL, 0);
    }
    else if (!strcmp(args[0], "buffer")) {
        for (int i = 0; i < os->line_length; i++) {
            printf("%s\n", os->lines[i]);
        }
    }
    else {
        char buffer[MAX_INPUT*2] = {0};
        snprintf(buffer, MAX_INPUT*2, "Command '%s' was not found\0", command);
        pushlineOS(&local_os, buffer);
    }
    return false;
}

void updateOS(OS_t* os)
{
    os->ostime += GetFrameTime()*2;
    
    if (!os->grabbed) {
        if (IsKeyPressed(KEY_E)) {
            printf("FUCKING GRAB THAT SHIT: %s\n", os->grabbed ? "yep" : "nope");
            os->grabbed = true;
            os->isskip = false;
            printf("FUCKING GRAB THAT SHIT: %s\n", os->grabbed ? "yep" : "nope");
            
            switch (os->program)
            {
                case CONSOLE:
                    consoleGrab(os);
                    break;
                case CHAT:
                    chatGrab(os);
                    break;
                case LOGIN:
                    loginGrab(os);
                    break;
                case OFFLINE:
                    offlineGrab(os);
                    break;
                case BOOT:
                    bootGrab(os);
                    break;
                default:
                    break;
            }
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
    
    // TODO: update
    switch (os->program)
    {
        case CONSOLE:
            consoleUpdate(os);
            break;
        case CHAT:
            chatUpdate(os);
            break;
        case LOGIN:
            loginUpdate(os);
            break;
        case OFFLINE:
            offlineUpdate(os);
            break;
        case BOOT:
            bootUpdate(os);
            break;
        default:
            break;
    }
    
}

void flashOS(OS_t* os, char lines[MAX_LINES][MAX_INPUT], int num_lines)
{
    if (lines == NULL) {
        for (int i = 0; i < MAX_LINES; i++) {
            memset(os->lines[i], '\0', MAX_INPUT);
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
        char newline[MAX_INPUT];
        char rest[strlen(line)-MAX_INPUT+2];
        
        memcpy(newline, &line[0], MAX_INPUT-1);
        newline[MAX_INPUT-1] = '\0';
        memcpy(rest, &line[MAX_INPUT-1], strlen(line)-MAX_INPUT+1);
        rest[strlen(line)-MAX_INPUT+1] = '\0';
        
        pushlineOS(os, newline);
        pushlineOS(os, rest);
        return;
    }
    if (os->line_length == MAX_LINES) {
        for (int i = 0; i < MAX_LINES-1; i++) {
            memcpy(os->lines[i], os->lines[i+1], strlen(os->lines[i+1]));
        }
    } else {
        os->line_length++;
    }
    memcpy(os->lines[os->line_length-1], line, strlen(line));
}

void drawOS(OS_t* os, Screen_t* scr)
{
    BeginTextureMode(scr->texture);
    DrawRectangle(0, 0, scr->texture.texture.width*screen_w_gl, scr->texture.texture.height*screen_h_gl, BLACK);
    // TODO: draw
    switch (os->program)
    {
        case CONSOLE:
            consoleDraw(os);
            break;
        case CHAT:
            chatDraw(os);
            break;
        case LOGIN:
            loginDraw(os);
            break;
        case OFFLINE:
            offlineDraw(os);
            break;
        case BOOT:
            bootDraw(os);
            break;
        default:
            break;
    }
    EndTextureMode();
}

/// Programs

// Console
void consoleUpdate(OS_t* os)
{
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
    } else if (IsKeyPressed(KEY_BACKSPACE)) {
        if (os->input_length > 0) {
            os->input[os->input_length-1] = 0;
            os->input[os->input_length] = '\0';
            os->input_length--;
        }
    } else if (k == 32) {
        if (os->input_length < MAX_INPUT) {
            os->input[os->input_length] = ' ';
            os->input_length++;
        }
    } else if (IsKeyPressed(KEY_ENTER)) {
        commandOS(os, os->input);
        memset(os->input,'\0',MAX_INPUT);
        os->input_length = 0;
    }
    else if (k > -1) {
        // printf("%d\n", k);
    }
}

void consoleDraw(OS_t* os)
{
    for (int i = 0; i < MAX_LINES; i++) {
        DrawText(os->lines[i], 5, 5+i*22, 20, GREEN);
    }
    int iw = MeasureText(FormatText(">%s", os->input), 20);
    DrawText(FormatText(">%s", os->input), 5, 5+os->line_length*22, 20, GREEN);
    if ((int)os->ostime%2==1) {
        DrawRectangle(5+iw+1, 5+os->line_length*22, 10, 20, GREEN);
    }
}

void consoleGrab(OS_t* os)
{
    
}

// Chat
void chatUpdate(OS_t* os)
{
    
}

void chatDraw(OS_t* os)
{
    
}

void chatGrab(OS_t* os)
{
    
}

// Offline
void offlineUpdate(OS_t* os)
{
    
}

void offlineDraw(OS_t* os)
{
    int ow = MeasureText("TERMINAL OFFLINE", 40);
    DrawText("TERMINAL OFFLINE", screen_w/2-ow/2, screen_h/2-20, 40, GRAY);
}

void offlineGrab(OS_t* os)
{
    os->program = BOOT;
}

// Boot
void bootUpdate(OS_t* os)
{
    
}

void bootDraw(OS_t* os)
{
    os->bootline_timeout += GetFrameTime();
    if (os->bootline_timeout>0.07f && os->bootline < MAX_READOUTS) {
        pushlineOS(os, boot_lines[os->bootline]);
        os->bootline++;
        os->bootline_timeout = 0;
    }
    for (int i = 0; i < MAX_LINES; i++) {
        DrawText(os->lines[i], 5, 5+i*22, 20, GREEN);
    }
    if (os->bootline == MAX_READOUTS && os->bootline_timeout>1) {
        if (strlen(os->username) == 0) {
            os->program = LOGIN;
        } else {
            os->program = CONSOLE;
        }
    }
}

void bootGrab(OS_t* os)
{
    
}

// Login
void loginUpdate(OS_t* os)
{
    int k = GetKeyPressed();
    bool k_is_allowed = false;
    for (int i = 0; i < strlen(clist); i++) {
        if (clist[i] == k) {
            k_is_allowed = true;
            break;
        }
    }
    if (k_is_allowed) {
        if (os->username_length < MAX_USERNAME) {
            os->username[os->username_length] = (char)k;
            os->username_length++;
        }
    } else if (IsKeyPressed(KEY_BACKSPACE)) {
        if (os->username_length > 0) {
            os->username[os->username_length-1] = '\0';
            // os->username[os->username_length] = '\0';
            os->username_length--;
        }
    } else if (k == 32) {
        // if (os->username_length < MAX_USERNAME) {
        //     os->username[os->username_length] = ' ';
        //     os->username_length++;
        // }
    } else if (IsKeyPressed(KEY_ENTER)) {
        flashOS(os,NULL,0);
        pushlineOS(os, FormatText("Welcome to your terminal, %s.", os->username));
        os->program = CONSOLE;
    }
    else if (k > -1) {
        // printf("%d\n", k);
    }
}

void loginDraw(OS_t* os)
{
    int w = MeasureText("ENTER USERNAME: ", 20);
    int iw = MeasureText(FormatText("%s", os->username), 20);
    DrawText("ENTER USERNAME: ", 5, screen_h/2-100, 20, GREEN);
    DrawText(FormatText("%-16s", os->username), 10+w, screen_h/2-100, 20, GREEN);
    DrawLineEx((Vector2){10+w, screen_h/2-81}, (Vector2){10+w*2, screen_h/2-81}, 3, GREEN);
    // DrawLine(10+w, screen_h/2+12, 10+w*2, screen_h/2+12, GREEN);
    // DrawLine(10+w, screen_h/2+13, 10+w*2, screen_h/2+13, GREEN);
    if ((int)os->ostime%2==1) {
        DrawRectangle(10+w+iw+1, screen_h/2-100, 10, 20, GREEN);
    }
}

void loginGrab(OS_t* os)
{
    
}

