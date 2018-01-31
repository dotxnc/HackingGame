#include "os.h"

static Shader terminal_shader;
static RenderTexture2D terminal_buffer;
static float terminal_time;
static int terminal_time_pos;
static int terminal_screensize_pos;

void initOS(OS_t* os)
{
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
    local_os.backspace_timer = 0.f;
    
    flashOS(&local_os, NULL, 0);
    memset(local_os.input,'\0',MAX_INPUT);
    memset(local_os.username,'\0',MAX_USERNAME);
    for (int i = 0; i < MAX_LINES; i++)
        memset(local_os.chatlog[i], '\0', MAX_INPUT);
    
    terminal_buffer = LoadRenderTexture(screen_w, screen_h);
    terminal_shader = LoadShader("assets/shaders/standard.vs", "assets/shaders/terminal.fs");
    terminal_time_pos = GetShaderLocation(terminal_shader, "time");
    terminal_screensize_pos = GetShaderLocation(terminal_shader, "screensize");
    
    int screensize[2] = {screen_w, screen_h};
    SetShaderValuei(terminal_shader, terminal_screensize_pos, screensize, 2);
    
}

void freeOS(OS_t* os)
{
    UnloadShader(terminal_shader);
}

bool commandOS(OS_t* os, char* command)
{
    if (os->program == CHAT) {
        if (command[0] == '/') {
            if (!strcmp(command, "/leave")) {
                os->program = CONSOLE;
                return true;
            }
            else {
                pushchatOS(os, FormatText(">>> Command %s was not found", command));
                return false;
            }
        }
        
        ChatPacket_t chatpacket;
        strcpy(chatpacket.chat, command);
        chatpacket.uid = network.client.uid;
        sendDataClient(&chatpacket, sizeof(ChatPacket_t), PACKET_CHAT);
        return true;
    }
    
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
            pushlineOS(os, "ATTEMPTING CONNECTION...");
        }
        return true;
    }
    else if (!strcmp(args[0], "test")) {
        for (int i = 0; i < 20; i++) {
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
    else if (!strcmp(args[0], "irc")) {
        os->program = CHAT;
    }
    else if (!strcmp(args[0], "help")) {
        pushlineOS(os, "COMMANDS:");
        pushlineOS(os, "SERVER <N>: RUNS SERVER ON PORT 'N'");
        pushlineOS(os, "CLIENT <I> <N>: CONNECTS TO 'I' ON PORT 'N'");
        pushlineOS(os, "CLEAR: RESETS THE BUFFER");
        pushlineOS(os, "IRC: CONNECTS TO IRC CHAT CHANNEL");
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
    terminal_time += GetFrameTime();
    SetShaderValue(terminal_shader, terminal_time_pos, &terminal_time, 1);
    os->backspace_timer += GetFrameTime();
    if (os->backspace_timer > 1) os->backspace_timer = 1;
    if (IsKeyUp(KEY_BACKSPACE)) os->backspace_timer = 1;
    
    if (!os->grabbed) {
        if (IsKeyPressed(KEY_E)) {
            os->grabbed = true;
            os->isskip = false;
            
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

void pushOS(OS_t* os, const char* line, bool ischat)
{
    if (strlen(line) > MAX_INPUT) {
        char newline[MAX_INPUT];
        char rest[strlen(line)-MAX_INPUT+2];
        
        memcpy(newline, &line[0], MAX_INPUT-1);
        newline[MAX_INPUT-1] = '\0';
        memcpy(rest, &line[MAX_INPUT-1], strlen(line)-MAX_INPUT+1);
        rest[strlen(line)-MAX_INPUT+1] = '\0';
        
        pushOS(os, newline, ischat);
        pushOS(os, rest, ischat);
        return;
    }
    if (os->line_length == MAX_LINES) {
        for (int i = 0; i < MAX_LINES-1; i++) {
            if (ischat)
                strcpy(os->chatlog[i], os->chatlog[i+1]);
            else
                strcpy(os->lines[i], os->lines[i+1]);
        }
    } else {
        if (ischat)
            os->chat_length++;
        else
            os->line_length++;
    }
    if (ischat)
        strcpy(os->chatlog[os->chat_length-1], line);
    else
        strcpy(os->lines[os->line_length-1], line);
}

void pushlineOS(OS_t* os, const char* line) {
    pushOS(os, line, false);
}

void pushchatOS(OS_t* os, const char* line) {
    pushOS(os, line, true);
}

void drawOS(OS_t* os, Screen_t* scr)
{
    BeginTextureMode(terminal_buffer);
        DrawRectangle(0, 0, scr->texture.texture.width*screen_w_gl, scr->texture.texture.height, DARKGRAY);
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
        
    BeginTextureMode(scr->texture);
        BeginShaderMode(terminal_shader);
            DrawRectangle(0, 0, terminal_buffer.texture.width, terminal_buffer.texture.height, DARKGRAY);
            DrawTextureRec(terminal_buffer.texture, (Rectangle){0, 0, -terminal_buffer.texture.width, -terminal_buffer.texture.height}, (Vector2){0, 0}, WHITE);
        EndShaderMode();
    EndTextureMode();
}

/// Programs

// Console
void consoleUpdate(OS_t* os)
{
    int k = GetKeyPressed();
    bool k_is_allowed = false;
    if (os->program == CONSOLE) {
        for (int i = 0; i < strlen(clist); i++) {
            if (clist[i] == k) {
                k_is_allowed = true;
                break;
            }
        }
    } else if (os->program == CHAT){
        for (int i = 0; i < strlen(chatclist); i++) {
            if (chatclist[i] == k) {
                k_is_allowed = true;
                break;
            }
        }
    }
    if (k_is_allowed) {
        if (os->input_length < MAX_INPUT) {
            os->input[os->input_length] = (char)k;
            os->input_length++;
        }
    } else if (IsKeyDown(KEY_BACKSPACE) && os->backspace_timer > BACKSPACE_TIMEOUT) {
        if (os->input_length > 0) {
            os->input[os->input_length-1] = '\0';
            os->input_length--;
            os->backspace_timer = 0;
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
}

void consoleDraw(OS_t* os)
{
    for (int i = 0; i < MAX_LINES; i++) {
        DrawText(os->lines[i], 5, 5+i*22, 20, WHITE);
    }
    int iw = MeasureText(FormatText(">%s", os->input), 20);
    DrawText(FormatText(">%s", os->input), 5, 5+os->line_length*22, 20, WHITE);
    if ((int)os->ostime%2==1) {
        DrawRectangle(5+iw+1, 5+os->line_length*22, 10, 20, WHITE);
    }
}

void consoleGrab(OS_t* os)
{
    
}

// Chat
void chatUpdate(OS_t* os)
{
    if (!network.client_running) {
        if (IsKeyPressed(KEY_ENTER)) {
            os->program = CONSOLE;
        }
        return;
    }
    consoleUpdate(os);
}

void chatDraw(OS_t* os)
{
    if (!network.client_running) {
        int tw = MeasureText("CLIENT NOT CONNECTED", 30);
        int rw = MeasureText("PRESS ENTER", 30);
        DrawText("CLIENT NOT CONNECTED", floor((screen_w)/2-tw/2), floor((screen_h)/2-80), 30, WHITE);
        DrawText("PRESS ENTER", floor((screen_w)/2-rw/2), floor((screen_h)/2-40), 30, WHITE);
        return;
    }
    
    for (int i = 0; i < MAX_LINES; i++) {
        DrawText(os->chatlog[i], 5, 5+i*22, 20, WHITE);
    }
    int iw = MeasureText(FormatText(">%s", os->input), 20);
    DrawText(FormatText(">%s", os->input), 5, 5+os->chat_length*22, 20, WHITE);
    if ((int)os->ostime%2==1) {
        DrawRectangle(5+iw+1, 5+os->chat_length*22, 10, 20, WHITE);
    }
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
        DrawText(os->lines[i], 5, 5+i*22, 20, WHITE);
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
    } else if (IsKeyDown(KEY_BACKSPACE) && os->backspace_timer > BACKSPACE_TIMEOUT) {
        if (os->username_length > 0) {
            os->username[os->username_length-1] = '\0';
            os->username_length--;
            os->backspace_timer = 0;
        }
    } else if (IsKeyPressed(KEY_ENTER)) {
        flashOS(os,NULL,0);
        pushlineOS(os, FormatText("Welcome to your p2 terminal, %s.", os->username));
        os->program = CONSOLE;
    }
}

void loginDraw(OS_t* os)
{
    int w = MeasureText("ENTER USERNAME: ", 20);
    int iw = MeasureText(FormatText("%s", os->username), 20);
    DrawText("ENTER USERNAME: ", 5, screen_h/2-100, 20, WHITE);
    DrawText(FormatText("%-16s", os->username), 10+w, screen_h/2-100, 20, WHITE);
    DrawLineEx((Vector2){10+w, screen_h/2-81}, (Vector2){10+w*2, screen_h/2-81}, 3, WHITE);
    if ((int)os->ostime%2==1) {
        DrawRectangle(10+w+iw+1, screen_h/2-100, 10, 20, WHITE);
    }
}

void loginGrab(OS_t* os)
{
    
}

