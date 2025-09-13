#include "ctype.h"
#include "Hal.h"
#include "string.h"
#include "kybrd.h"
#include "../Kernel/DebugDisplay.h"


// Keyboard Encoder Configuration
enum KYBRD_ENCODER_IO {
    KYBRD_ENC_INPUT_BUF     = 0x60,
    KYBRD_ENC_CMD_RED       = 0x60
};

enum KYBRD_ENC_CMDS {
    KYBRD_ENC_CMD_SET_LED               = 0xED,
    KYBRD_ENC_CMD_ECHO                  = 0xEE,
    KYBRD_ENC_CMD_SCAN_CODE_SET         = 0xF0,
    KYBRD_ENC_CMD_ID                    = 0xF2,
    KYBRD_ENC_CMD_AUTODELAY             = 0xF3,
    KYBRD_ENC_CMD_ENABLE                = 0xF4,
    KYBRD_ENC_CMD_RESEWAIT              = 0xF5,
    KYBRD_ENC_CMD_RESETSCAN             = 0xF6,
    KYBRD_ENC_CMD_ALL_AUTO              = 0xF7,
    KYBRD_ENC_CMD_ALL_MAKEBREAK         = 0xF8,
    KYBRD_ENC_CMD_ALL_MAKEONLY          = 0xF9,
    KYBRD_ENC_CMD_ALL_MAKEBREAK_AUTO    = 0xFA,
    KYBRD_ENC_CMD_SINGLE_AUTOREPEAT     = 0xFB,
    KYBRD_ENC_CMD_SINGLE_MAKEBREAK      = 0xFC,
    KYBRD_ENC_CMD_SINGLE_BREAKONLY      = 0xFD,
    KYBRD_ENC_CMD_RESEND                = 0xFE,
    KYBRD_ENC_CMD_RESET                 = 0xFF
};


// Keyboard Controller Configuration 

enum KYBRD_CTRL_IO {
    KYBRD_CTRL_STATS_REG   = 0x64,
    KYBRD_CTRL_CMD_REG     = 0x64  
};


enum KYBRD_CTRL_STATS_MASK {
	KYBRD_CTRL_STATS_MASK_OUT_BUF	=	1,		//00000001
	KYBRD_CTRL_STATS_MASK_IN_BUF	=	2,		//00000010
	KYBRD_CTRL_STATS_MASK_SYSTEM	=	4,		//00000100
	KYBRD_CTRL_STATS_MASK_CMD_DATA	=	8,		//00001000
	KYBRD_CTRL_STATS_MASK_LOCKED	=	0x10,	//00010000
	KYBRD_CTRL_STATS_MASK_AUX_BUF	=	0x20,	//00100000
	KYBRD_CTRL_STATS_MASK_TIMEOUT	=	0x40,	//01000000
	KYBRD_CTRL_STATS_MASK_PARITY	=	0x80	//10000000
};

enum KYBRD_CTRL_CMDS {
    KYBRD_CTRL_CMD_READ             = 0x20,
    KYBRD_CTRL_CMD_WRITE            = 0x60,
    KYBRD_CTRL_CMD_SELF_TEST        = 0xAA,
    KYBRD_CTRL_CMD_INTERFACE_TEST   = 0xAB,
    KYBRD_CTRL_CMD_DISABLE          = 0xAD,
    KYBRD_CTRL_CMD_ENABLE           = 0xAE,
    KYBRD_CTRL_CMD_READ_IN_PORT     = 0xC0,
    KYBRD_CTRL_CMD_READ_OUT_PORT    = 0xD0,
    KYBRD_CTRL_CMD_WRITE_OUT_PORT   = 0xD1,
    KYBRD_CTRL_CMD_READ_TEST_INPUT  = 0xE0,
    KYBRD_CTRL_CMD_SYSTEM_RESET     = 0xFE,
    KYBRD_CTRL_CMD_MOUSE_DISABLE    = 0xA7,
    KYBRD_CTRL_CMD_MOUSE_ENABLE     = 0xA8,
    KYBRD_CTRL_CMD_MOUSE_PORT_TEST  = 0xA9,
    KYBRD_CTRL_CMD_MOUSE_WRITE      = 0xD4
};


// scan error codes ------------------------------------------
enum KYBRD_ERROR {
    KYBRD_ERR_BIF_OVERRUN       = 0,
    KYBRD_ERR_ID_RET            = 0x83AB,
    KYBRD_ERR_BAT               = 0xAA,
    KYBRD_ERR_ECHO_RET          = 0xEE,
    KYBRD_ERR_ACK               = 0xFA,
    KYBRD_ERR_BAT_FAILED        = 0xFC,
    KYBRD_ERR_DIAG_FAILED       = 0xFD,
    KYBRD_ERR_RESEND_CMD        = 0xFE,
    KYBRD_ERR_KEY               = 0xFF
};

// ! Current Scancode 
volatile char _scancode = 0;

// ! Lock Key
static bool _numlock, _scrolllock, _capslock;

// ! Shift , ALT, and CTRL keys current state
static bool _shift, _alt, _ctrl;

// ! Set if keyboard error 
static int _kkybrd_error        = 0;

// ! Set if Basic Assurance Test (BAT) failed 
static bool _kkybrd_bat_res     = false;

// ! Set if Diagnostic failed
static bool _kkybrd_diag_res   = false;

// ! Set if system should resent last command
static bool _kkybrd_resend_res  = false;

// ! Set if keyboard is disabled 
static bool _kkybrd_disable     = false;

static int _kkybrd_scancode_std [] = {
    // Key          // Scancode
    KEY_UNKNOWN,    // 0
    KEY_ESCAPE,     // 1 
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_MINUS,
    KEY_EQUAL,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_T,
    KEY_Y,
    KEY_U,
    KEY_I,
    KEY_O,
    KEY_P,
    KEY_LEFTBRACKET,
    KEY_RIGHTBRACKET,
    KEY_RETURN,
    KEY_LCTRL,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_SEMICOLON,
    KEY_QUOTE,
    KEY_GRAVE,         // `
    KEY_LSHIFT,
    KEY_BACKSLASH,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M,
    KEY_COMMA,
    KEY_DOT,
    KEY_SLASH,
    KEY_RSHIFT,
    KEY_KP_ASTERISK,
    KEY_LALT,
    KEY_SPACE,
    KEY_CAPSLOCK,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10, 
    KEY_KP_NUMLOCK,
    KEY_SCROLLLOCK,
	KEY_HOME,		//0x47
	KEY_KP_8,		//0x48	//keypad up arrow
	KEY_PAGEUP,		//0x49
	KEY_KP_2,		//0x50	//keypad down arrow
	KEY_KP_3,		//0x51	//keypad page down
	KEY_KP_0,		//0x52	//keypad insert key
	KEY_KP_DECIMAL,	//0x53	//keypad delete key
	KEY_UNKNOWN,	//0x54
	KEY_UNKNOWN,	//0x55
	KEY_UNKNOWN,	//0x56
	KEY_F11,		//0x57
	KEY_F12			//0x58
};


const int INVALID_SCANCODE = 0;

// Internal functions
uint8_t     kybrd_ctrl_read_status();
void        kybrd_ctrl_send_cmd(uint8_t);
uint8_t     kybrd_enc_read_buf();
void        _cdecl kybrd_enc_send_cmd(uint8_t);


uint8_t kybrd_ctrl_read_status(){
    return inportb(KYBRD_CTRL_STATS_REG);
}

void kybrd_ctrl_send_cmd(uint8_t cmd){
    while (true){
        // Input buffer full, dont write yet
        if ((kybrd_ctrl_read_status() & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
            break;
    }
    outportb(KYBRD_CTRL_CMD_REG, cmd);
}

uint8_t kybrd_enc_read_buf(){
    return inportb(KYBRD_ENC_INPUT_BUF);
}

// ! Both Controller and Encoder share input buffer 
// ! Since Encoder cmd are first sent to controller 
void _cdecl kybrd_enc_send_cmd(uint8_t cmd){
    while (true){
        if ((kybrd_ctrl_read_status() & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
            break;
    }
    outportb(KYBRD_ENC_CMD_RED, cmd);
}



static void __cdecl kybrd_handler_c(void){
    
    static bool _extended = false;
    int code = 0;

    if (kybrd_ctrl_read_status() & KYBRD_CTRL_STATS_MASK_OUT_BUF){
        code = kybrd_enc_read_buf();
        // Some Special key is in the form of 0xE0 + Scancode
        if (code == 0xE0 || code == 0xE1)
            _extended = true;
        else{
            _extended = false;
            
            // Break Code = 0x80 | Make Code
            if (code & 0x80){
                code -= 0x80;
                int key = _kkybrd_scancode_std[code];
                switch (key){
                    case KEY_LCTRL:
                    case KEY_RCTRL:
                        _ctrl = false;
                        break;
                    case KEY_LSHIFT:
                    case KEY_RSHIFT:
                        _shift = false;
                        break;
                    
                    case KEY_LALT:
                    case KEY_RALT:
                        _alt = false;
                        break;
                }
            }

            // Make Code
            else{
                _scancode = code;
                int key = _kkybrd_scancode_std[code];
                switch (key){
                    case KEY_LCTRL:
                    case KEY_RCTRL:
                        _ctrl = true;
                        break;

                    case KEY_LSHIFT:
                    case KEY_RSHIFT:
                        _shift = true;
                        break;
                    
                    case KEY_LALT:
                    case KEY_RALT:
                        _alt = true;
                        break;
                    
                    case KEY_CAPSLOCK:
                        _capslock = (_capslock) ? false : true;
                        kkybrd_set_leds(_numlock, _capslock, _scrolllock);
                        break;

                    case KEY_KP_NUMLOCK:
                        _numlock = (_numlock) ? false : true;
                        kkybrd_set_leds(_numlock, _capslock, _scrolllock);
                        break;
                    
                    case KEY_SCROLLLOCK:
                        _scrolllock = (_scrolllock) ? false : true;
                        kkybrd_set_leds(_numlock, _capslock, _scrolllock);
                        break;
                }

                switch (code){
                    case KYBRD_ERR_BAT_FAILED:
                        _kkybrd_bat_res = false;
                        break;
                    case KYBRD_ERR_DIAG_FAILED:
                        _kkybrd_diag_res = false;
                        break;
                    case KYBRD_ERR_RESEND_CMD:
                        _kkybrd_resend_res = true;
                        break;
                }
            }
  
        }
    }
    interruptdone(0);
}

extern "C" interrupt void i86_kybrd_irq(){
    #ifdef _MSC_VER
    _asm {
        pushad
        cli
        call kybrd_handler_c
        sti
        popad
        iretd
    } 
    #endif
}


bool kkybrd_get_scroll_lock(){
    return _scrolllock;
}
bool kkybrd_get_num_lock(){
    return _numlock;
}
bool kkybrd_get_caplock(){
    return _capslock;
}
bool kkybrd_get_alt(){
    return _alt;
}
bool kkybrd_get_ctrl(){
    return _ctrl;
}
bool kkybrd_get_shift(){
    return _shift;
}
void kkybrd_ignore_resend(){
    _kkybrd_resend_res = false;
}
bool kkybrd_check_resend(){
    return _kkybrd_resend_res;
}
bool kkybrd_get_diagnostic_res(){
    return _kkybrd_diag_res;
}
bool kkybrd_get_bat_res(){
    return _kkybrd_bat_res;
}

uint8_t kkybrd_get_last_scan(){
    return _scancode;
}

KEYCODE kkybrd_get_last_key(){
    if (_scancode != INVALID_SCANCODE)
        return (KEYCODE)_kkybrd_scancode_std[_scancode];
    else return KEY_UNKNOWN;
}

void kkybrd_discard_last_key(){
    _scancode = INVALID_SCANCODE;
}

void kkybrd_set_leds (bool num, bool caps, bool scroll){
    uint8_t data = 0;
    data = scroll ? (data | 1) : (data & ~1);
    data = num    ? (data | 2) : (data & ~2);
    data = caps   ? (data | 4) : (data & ~4);
    kybrd_enc_send_cmd(KYBRD_ENC_CMD_SET_LED);
    kybrd_enc_send_cmd(data);
}

char kkybrd_key_to_ascii(KEYCODE code){
    uint8_t key = code;
    if (isascii(key)){
        if (_shift || _capslock){
            if (key >= 'a' && key <= 'z')
                key -= 32;
        }

        if (_shift){
            if (key >= '0' && key <= '9'){
                switch (key){
                    case '1':
                        key = KEY_EXCLAMATION;
                        break;
                    case '2':
                        key = KEY_AT;
                        break;
                    case '3':
                        key = KEY_HASH;
                        break;
                    case '4':
                        key = KEY_DOLLAR;
                        break;
                    case '5':
                        key = KEY_PERCENT;
                        break;
                    case '6':
                        key = KEY_CARRET;
                        break;
                    case '7':
                        key = KEY_AMPERSAND;
                        break;
                    case '8':
                        key = KEY_ASTERISK;
                        break;
                    case '9':
                        key = KEY_LEFTPARENTHESIS;
                        break;
                    case '0':
                        key = KEY_RIGHTPARENTHESIS;
                        break;
                }
            }else{
                switch (key){
                    case KEY_COMMA:
                        key = KEY_LESS;
                        break;
                    case KEY_DOT:
                        key = KEY_GREATER;
                        break;
                    case KEY_SLASH:
                        key = KEY_QUESTION;
                        break;
                    case KEY_QUOTE:
                        key = KEY_QUOTEDOUBLE;
                        break;
                    case KEY_SEMICOLON:
                        key = KEY_COLON;
                        break;
                    case KEY_LEFTBRACKET:
                        key = KEY_LEFTCURL;
                        break;
                    case KEY_RIGHTBRACKET:
                        key = KEY_RIGHTCURL;
                        break;
                    case KEY_GRAVE:
                        key = KEY_TILDE;
                        break;
                    case KEY_MINUS:
                        key = KEY_UNDERSCORE;
                        break;
                    case KEY_EQUAL:
                        key = KEY_PLUS;
                        break;
                    case KEY_BACKSLASH:
                        key = KEY_BAR;
                        break;
                }
            }
        }
        return key; 
    }
    return 0;
}
void kkybrd_disable(){
    kybrd_ctrl_send_cmd(KYBRD_CTRL_CMD_DISABLE);
    _kkybrd_disable = true;
}
void kkybrd_enable(){
    kybrd_ctrl_send_cmd(KYBRD_CTRL_CMD_ENABLE);
    _kkybrd_disable = false;
}
bool kkybrd_is_disabled(){
    return _kkybrd_disable;
}

void kkybrd_reset_system(){
    // ! Write 11111110 to the output port 
    // ! Output port is a 8 bit register in the keyboard controller
    // ! It send hardware signal to the system motherboard
    // ! i.e. A20, reset CPU ...etc
    kybrd_ctrl_send_cmd(KYBRD_CTRL_CMD_WRITE_OUT_PORT);
    kybrd_enc_send_cmd(KYBRD_CTRL_CMD_SYSTEM_RESET);
}

bool kkybrd_self_test(){
    kybrd_ctrl_send_cmd(KYBRD_CTRL_CMD_SELF_TEST);
    while (true){
        // Data returned
        if (kybrd_ctrl_read_status() & KYBRD_CTRL_STATS_MASK_OUT_BUF)
            break;
    }
    return (kybrd_enc_read_buf() & 0x55) ? true : false; 
}

void kkybrd_install(int irq){
    setvect(irq, i86_kybrd_irq); // IRQ 1 should be intno 33

    //! Assume BAT test is good. 
    //! If there is a problem, the IRQ handler where catch the error
    _kkybrd_bat_res = true;
    _scancode = 0;

    _numlock = _scrolllock = _capslock = false;
    // kkybrd_set_leds(false,false,false);

    _shift = _alt = _ctrl = false;
}
