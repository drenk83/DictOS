__asm("jmp kmain");

#define VIDEO_BUF_PTR (0xb8000)
#define IDT_TYPE_INTR (0x0E)
#define IDT_TYPE_TRAP (0x0F)
// Селектор секции кода, установленный загрузчиком ОС/
#define GDT_CS (0x8)
#define PIC1_PORT (0x20)

#define CURSOR_PORT (0x3D4)
#define VIDEO_WIDTH (80)
#define VIDEO_HEIGHT (25)

#define START_POSITION (0)
#define START_STRNUM (0)
#define MAX_COMMAND_LENGTH (40)

char userinput[40];
char cmd_command[20];
char cmd_arg[30];
char loader_str[27];

int user_color = 0x02;
int cmd_color = 0x07;

unsigned int cur_cursor = 0; // позиция курсора
unsigned int current_strnum = 0;  // Количество символов в строке
unsigned int loader_len = 0; // Длина строки из загрузчика

void out_str();
void default_intr_handler();
void intr_reg_handler();

void intr_init();
void intr_start();
void intr_enable();
void intr_disable();
void keyb_init();

void keyb_handler();
void keyb_process_keys();

void cursor_moveto();

void from_loader();
void on_key();
void backspace_key();
void enter_key();
void print_symbol();
char scan_code_to_ascii();

void parsing();
void command_handler();
void clr_scr();
void info_command();
void shutdown_command();

bool strcmp(unsigned char* str1, const char* str2)
{
    while (*str1 == *str2 && *str1 != 0 && *str1 != ' ' && *str2 != 0)
    {
        str1 += 2;
        str2++;
    }
    if (*str1 == *str2)
        return true;
    return false;
}

void out_str(int color, const char* ptr, unsigned int strnum)
{
    unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
    video_buf += VIDEO_WIDTH * 2 * strnum;
    while (*ptr)
    {
        video_buf[0] = (unsigned char) *ptr; // Символ (код)
        video_buf[1] = color; // Цвет символа и фона
        video_buf += 2;
        ptr++;
    }
}

const char* g_test = "This is test string.";

extern "C" int kmain()
{
    intr_disable();
    intr_init();
    keyb_init();

    intr_start();
    intr_enable();
    //const char* hello = "Welcome to HelloWorldOS (gcc edition)!";
    // Вывод строки
    // out_str(0x07, hello, 0);
    // out_str(0x07, g_test, 1);
    // Бесконечный цикл
    while(1)
    {
        asm("hlt");
    }
    return 0;
}

// Структура описывает данные об обработчике прерывания
struct idt_entry
{
    unsigned short base_lo;
    // Младшие биты адреса обработчика
    unsigned short segm_sel;
    // Селектор сегмента кода
    unsigned char always0;
    // Этот байт всегда 0
    unsigned char flags;
    // Флаги тип. Флаги: P, DPL, Типы - это константы - IDT_TYPE...
    unsigned short base_hi;
    // Старшие биты адреса обработчика
} __attribute__((packed)); // Выравнивание запрещено

// Структура, адрес которой передается как аргумент команды lidt
struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)); // Выравнивание запрещено

struct idt_entry g_idt[256]; // Реальная таблица IDT
struct idt_ptr g_idtp;
// Описатель таблицы для команды lidt
// Пустой обработчик прерываний. Другие обработчики могут быть реализованы по этому шаблону
void default_intr_handler()
{
    asm("pusha");
    // ... (реализация обработки)
    asm("popa; leave; iret");
}

typedef void (*intr_handler)();
void intr_reg_handler(int num, unsigned short segm_sel, unsigned short
flags, intr_handler hndlr)
{
    unsigned int hndlr_addr = (unsigned int) hndlr;
    g_idt[num].base_lo = (unsigned short) (hndlr_addr & 0xFFFF);
    g_idt[num].segm_sel = segm_sel;
    g_idt[num].always0 = 0;
    g_idt[num].flags = flags;
    g_idt[num].base_hi = (unsigned short) (hndlr_addr >> 16);
}

// Функция инициализации системы прерываний: заполнение массива с адресами обработчиков
void intr_init()
{
    int i;
    int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
    for(i = 0; i < idt_count; i++)
    intr_reg_handler(i, GDT_CS, 0x80 | IDT_TYPE_INTR,
    default_intr_handler); // segm_sel=0x8, P=1, DPL=0, Type=Intr
}

void intr_start()
{
    int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);
    g_idtp.base = (unsigned int) (&g_idt[0]);
    g_idtp.limit = (sizeof (struct idt_entry) * idt_count) - 1;
    asm("lidt %0" : : "m" (g_idtp) );
}

void intr_enable()
{
    asm("sti");
}

void intr_disable()
{
    asm("cli");
}

static inline unsigned char inb (unsigned short port) // Чтение из порта
{
    unsigned char data;
    asm volatile ("inb %w1, %b0" : "=a" (data) : "Nd" (port));
    return data;
}
static inline void outb (unsigned short port, unsigned char data) // Запись
{
    asm volatile ("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

void from_loader()
{
    char *ptr = (char *)0x9000;
    
    for(int i = 0; i < 26; i++)
    {
        if(*ptr != '_')
        {
            loader_str[loader_len] = *ptr;
            loader_len++;
        }
        ptr++;
    }
    loader_str[loader_len] = '\0';
}

void keyb_init()
{
    // Регистрация обработчика прерывания
    intr_reg_handler(0x09, GDT_CS, 0x80 | IDT_TYPE_INTR, keyb_handler);
    // segm_sel=0x8, P=1, DPL=0, Type=Intr
    // Разрешение только прерываний клавиатуры от контроллера 8259
    outb(PIC1_PORT + 1, 0xFF ^ 0x02); // 0xFF - все прерывания, 0x02 - бит IRQ1 (клавиатура).
    // Разрешены будут только прерывания, чьи биты установлены в 0
}

void keyb_handler()
{
    asm("pusha");
    // Обработка поступивших данных
    keyb_process_keys();
    // Отправка контроллеру 8259 нотификации о том, что прерывание обработано
    outb(PIC1_PORT, 0x20);
    asm("popa; leave; iret");
}

void keyb_process_keys()
{
// Проверка что буфер PS/2 клавиатуры не пуст (младший бит присутствует)
    if (inb(0x64) & 0x01)
    {
        unsigned char scan_code;
        unsigned char state;
        scan_code = inb(0x60); // Считывание символа с PS/2 клавиатуры
        if (scan_code < 128) // Скан-коды выше 128 - это отпускание клавиши
            on_key(scan_code);
    }
}

void on_key(unsigned char scan_code)
{
    if (scan_code == 14)
        backspace_key();
    else if (scan_code == 28)
        enter_key();
    else if (cur_cursor - 2 < MAX_COMMAND_LENGTH)
        print_symbol(scan_code);
}

void backspace_key()
{
    if (cur_cursor >= START_POSITION)
    {
        cur_cursor--;
        unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
        video_buf += 2 * (current_strnum * VIDEO_WIDTH + cur_cursor);
        video_buf[0] = 0;
        cursor_moveto(current_strnum, cur_cursor);
    }
}

void enter_key()
{
    command_handler();
    cur_cursor = START_POSITION;
    userinput[cur_cursor] = '\0';
    out_str(user_color, userinput, current_strnum);
    cursor_moveto(current_strnum, cur_cursor);
}

void print_symbol(unsigned char scan_code)
{
    char input = scan_code_to_ascii(scan_code);
    if (input == 0)
        return;
    userinput[cur_cursor] = input;
    cur_cursor++;
    userinput[cur_cursor] = input;
    out_str(user_color, userinput, current_strnum);
    cursor_moveto(current_strnum, cur_cursor);
}

void cursor_moveto(unsigned int strnum, unsigned int pos)
{
    unsigned short new_pos = (strnum * VIDEO_WIDTH) + pos;
    outb(CURSOR_PORT, 0x0F);
    outb(CURSOR_PORT + 1, (unsigned char)(new_pos & 0xFF));
    outb(CURSOR_PORT, 0x0E);
    outb(CURSOR_PORT + 1, (unsigned char)( (new_pos >> 8) & 0xFF));
}

void parsing()
{
    int i = START_POSITION;
    int command_start;
    
    while(i < cur_cursor && userinput[i] != ' ')
    {
        cmd_command[i] = userinput[i];
        i++;
    }

    cmd_command[i] = '\0';
    i++;
    command_start = i;

    while(i < cur_cursor && userinput[i] != ' ' && userinput[i] != '\0')
    {
        cmd_arg[i - command_start] = userinput[i];
        i++;
    }
    cmd_arg[i - command_start] = '\0';
}

void command_handler()
{
    unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
    video_buf += 2 * (current_strnum * VIDEO_WIDTH + 2);

    parsing();

    current_strnum++;
    cur_cursor = 0;

    if(strcmp(cmd_command, "clear\0") == 0)
        clr_scr();
    else if (strcmp(cmd_command, "info\0") == 0)
        info_command();  
    else if (strcmp(cmd_command, "shutdown\0") == 0)
        shutdown_command();  

    else if (strcmp(cmd_command, "translate\0") == 0)
        int nigger;
        //translate_command();
    else if(strcmp(cmd_command, "dictinfo\0") == 0)
        int nigger;
        //dictinfo_command();
    else if(strcmp(cmd_command, "wordstat\0") == 0)
        int nigger;
        //wordstat_command();
    
    else
        unknown_command();
}

void clr_scr()
{
    unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
    for (int i = 0; i <= VIDEO_WIDTH * VIDEO_HEIGHT * 2; i++)
        video_buf[i] = 0;
    cur_cursor = START_POSITION;
    current_strnum = START_STRNUM;
}

void info_command()
{
    if (current_strnum + 4 >= VIDEO_HEIGHT)
    {
        clr_scr();
        current_strnum = 0;
    }
    out_str(user_color, "DictOS. Developer: Egor Chuprov, 4831001/10002, SpbPU, 2023", 0);
    current_strnum++;
    out_str(user_color, "Compilers: loader: yasm, kernel: gcc", 0);
    current_strnum++;
    out_str(user_color, "Bootloader parameter: ", 0);
    cur_cursor = 22;
    out_str(user_color, loader_str, 1);
    cur_cursor = 0;
}

static inline void shutdown_command()
{
    outw (0x604, 0x2000);
}

void unknown_command()
{
    if (current_strnum + 2 >= VIDEO_HEIGHT)
    {
        clr_scr();
        current_strnum = 0;
    }
    out_str(user_color, "Error: command not recognized", 0);
}


// Добавить + - = /
char scan_code_to_ascii(unsigned char scan_code) {
    unsigned char symbol;

    switch (scan_code) {
    case 0x02: return '1';
    case 0x03: return '2';
    case 0x04:
    symbol = '3';
    break;
    case 0x05:
    symbol = '4';
    break;
    case 0x06:
    symbol = '5';
    break;
    case 0x07:
    symbol = '6';
    break;
    case 0x08:
    symbol = '7';
    break;
    case 0x09:
    symbol = '8';
    break;
    case 0x0A:
    symbol = '9';
    break;
    case 0x0B:
    symbol = '0';
    break;
    case 0x10:
    symbol = 'q';
    break;
    case 0x11:
    symbol = 'w';
    break;
    case 0x12:
    symbol = 'e';
    break;
    case 0x13:
    symbol = 'r';
    break;
    case 0x14:
    symbol = 't';
    break;
    case 0x15:
    symbol = 'y';
    break;
    case 0x16:
    symbol = 'u';
    break;
    case 0x17:
    symbol = 'i';
    break;
    case 0x18:
    symbol = 'o';
    break;
    case 0x19:
    symbol = 'p';
    break;
    case 0x1E:
    symbol = 'a';
    break;
    case 0x1F:
    symbol = 's';
    break;
    case 0x20:
    symbol = 'd';
    break;
    case 0x21:
    symbol = 'f';
    break;
    case 0x22:
    symbol = 'g';
    break;
    case 0x23:
    symbol = 'h';
    break;
    case 0x24:
    symbol = 'j';
    break;
    case 0x25:
    symbol = 'k';
    break;
    case 0x26:
    symbol = 'l';
    break;
    case 0x2C:
    symbol = 'z';
    break;
    case 0x2D:
    symbol = 'x';
    break;
    case 0x2E:
    symbol = 'c';
    break;
    case 0x2F:
    symbol = 'v';
    break;
    case 0x30:
    symbol = 'b';
    break;
    case 0x31:
    symbol = 'n';
    break;
    case 0x32:
    symbol = 'm';
    break;
    default:
    symbol = 0;
    break;
    }

    return symbol;
}