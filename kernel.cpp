__asm("jmp kmain");

#define VIDEO_BUF_PTR (0xb8000)
#define IDT_TYPE_INTR (0x0E)
#define IDT_TYPE_TRAP (0x0F)
// Селектор секции кода, установленный загрузчиком ОС
#define GDT_CS (0x8)
#define PIC1_PORT (0x20)

#define CURSOR_PORT (0x3D4)
#define VIDEO_WIDTH (80)

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


void out_str(int color, const char* ptr, unsigned int strnum)
{
    unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
    video_buf += 80*2 * strnum;
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
    const char* hello = "Welcome to HelloWorldOS (gcc edition)!";
    // Вывод строки
    out_str(0x07, hello, 0);
    out_str(0x07, g_test, 1);
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
        if (scan_code < 128) int nigger;// Скан-коды выше 128 - это отпускание клавиши
            //on_key(scan_code);
    }
}

void cursor_moveto(unsigned int strnum, unsigned int pos)
{
    unsigned short new_pos = (strnum * VIDEO_WIDTH) + pos;
    outb(CURSOR_PORT, 0x0F);
    outb(CURSOR_PORT + 1, (unsigned char)(new_pos & 0xFF));
    outb(CURSOR_PORT, 0x0E);
    outb(CURSOR_PORT + 1, (unsigned char)( (new_pos >> 8) & 0xFF));
}