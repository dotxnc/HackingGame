#include "os.h"

void initOS(OS_t* os)
{
}

void freeOS(OS_t* os)
{
}

bool commandOS(OS_t* os, char* command)
{
    if (!strcmp(command, "server")) {
        // TODO: start server
        return true;
    }
    if (!strcmp(command, "client")) {
        // TODO: start client
        return true;
    }
    return false;
}

void updateOS(OS_t* os)
{
    os->ostime += GetFrameTime();
    
    if (!os->grabbed) {
        if (IsKeyPressed(KEY_E)) {
            os->grabbed = true;
            os->isskip = false;
            printf("Pressed use.\n");
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
    } else if (k == KEY_ESCAPE) {
        for (int k = 0; k < 52; k++) {
            os->input[k] = 0;
            os->input_length = 0;
        }
    } else if (k > -1) {
        printf("%d\n", k);
    }
}

void flashOS(OS_t* os, char lines[MAX_LINES][MAX_INPUT], int num_lines)
{
    os->line_length = 0;
    for (int i = 0; i < num_lines; i++) {
        pushlineOS(os, lines[i]);
        os->line_length++;
    }
    
}

void pushlineOS(OS_t* os, char line[MAX_INPUT])
{
    if (os->line_length == MAX_LINES) {
        for (int i = 0; i < MAX_LINES-1; i++) {
            strcpy(os->lines[i], os->lines[i+1]);
        }
    }
    strcpy(os->lines[os->line_length], line);
}

void drawOS(OS_t* os, Screen_t* scr)
{
    DrawRectangle(0, 0, scr->texture.texture.width*screen_w_gl, scr->texture.texture.height*screen_h_gl, BLACK);
    for (int i = 0; i < 40; i++) {
        DrawText(os->lines[i], 5, 5+i*22, 20, GREEN);
    }
    int iw = MeasureText(FormatText(">%s", os->input), 20);
    DrawText(FormatText(">%s", os->input), 5, 5+os->line_length*22, 20, GREEN);
    if ((int)os->ostime%2==1) {
        DrawRectangle(5+iw+1, 5+os->line_length*22, 10, 20, GREEN);
    }
}
