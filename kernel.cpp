// Эта инструкция обязательно должна быть первой, т.к. этот код компилируется в бинарный, 
// и загрузчик передает управление по адресу первой инструкции бинарного
__asm("jmp kmain");

#define VIDEO_BUF_PTR (0xb8000)

#define IDT_TYPE_INTR (0x0E)
#define IDT_TYPE_TRAP (0x0F)

#define GDT_CS (0x8)

#define PIC1_PORT (0x20)

#define CURSOR_PORT (0x3D4)
#define VIDEO_WIDTH (80)

#define MAX_COMMAND_LENGTH (40)
#define VIDEO_HEIGHT (25)

#define COMMAND_START_POS (8)  // Start position for cusror
#define COMMAND_START_STRNUM (0)

#define COLOR (0x0E)
#define USR_COLOR (0x07)
#define TOTAL_WORDS (504)

unsigned int current_pos = 0;
unsigned int current_strnum = 0;
unsigned int count_usrinput = 0;
unsigned int chosen_words = 0;
unsigned int boot_input_length = 0;
unsigned char scan_code;
char usrinput[40];
char dictionary[27];
int start_point[27];
unsigned int for_lcg = 0;
unsigned int x_lcg = 0;
int strlen_arg = 0;
int shift;
char buffer[40];
char tmp[40];
char cmd_command[40];
char cmd_arg[40];

char* out_info;

const char* dict[504][2] =
{
    {"a", "un"},
    {"able", "capaz"},
    {"about", "acerca"},
    {"above", "por encima de"},
    {"act", "acto"},
    {"add", "aNadir"},                  // N
    {"after", "despuEs"},
    {"again", "de nuevo"},
    {"against", "contra"},
    {"ago", "aire"},
    {"air", "ostaa"},
    {"all", "todo"},
    {"also", "tambiEn"},               // E
    {"always", "siempre"},
    {"among", "entre"},
    {"an", "un"},
    {"and", "y"},
    {"animal", "animal"},
    {"answer", "respuesta"},
    {"any", "cualquier"},
    {"appear", "aparecerA"},          // A
    {"are", "son"},
    {"area", "zona"},
    {"as", "como"},
    {"ask", "preguntar"},
    {"at", "en"},
    {"back", "espalda"},
    {"base", "base de"},
    {"be", "ser"},
    {"beauty", "belleza"},
    {"bed", "cama"},
    {"been", "estado"},
    {"before", "antes"},
    {"began", "comenzO"},            // O
    {"begin", "comenzar"},
    {"behind", "detrAs"},
    {"best", "mejor"},
    {"better", "mejor"},
    {"between", "entre"},
    {"big", "grande"},
    {"bird", "pAjaro"},            // A
    {"black", "negro"},
    {"blue", "azul"},
    {"boat", "barco"},
    {"body", "cuerpo"},
    {"book", "libro"},
    {"both", "ambos"},
    {"box", "caja"},
    {"boy", "niNo"},
    {"bring", "traer"},
    {"brother", "hermano"},
    {"brought", "traido"},
    {"build", "construir"},
    {"busy", "ocupado"},
    {"but", "pero"},
    {"by", "por"},
    {"call", "llamada"},
    {"came", "vino"},
    {"can", "lata"},
    {"car", "coche"},
    {"care", "cuidado"},
    {"carry", "llevar"},
    {"cause", "causa"},
    {"center", "centro"},
    {"certain", "cierto"},
    {"change", "cambio"},
    {"check", "comprobar"},
    {"children", "niNos"},
    {"city", "ciudad"},
    {"class", "clase"},
    {"clear", "claro"},
    {"close", "cerrar"},
    {"cold", "frio"},
    {"color", "colores"},
    {"come", "venir"},
    {"common", "comUn"},               // U
    {"complete", "completo"},
    {"contain", "contener"},
    {"correct", "correcta"},
    {"could", "podia"},
    {"country", "pais"},
    {"course", "curso"},
    {"cover", "cubierta"},
    {"cross", "cruzar"},
    {"cry", "grito"},
    {"cut", "cortada"},
    {"dark", "oscuro"},
    {"day", "dia"},
    {"decide", "decidir"},
    {"deep", "profunda"},
    {"develop", "desarrollar"},
    {"did", "hizo"},
    {"differ", "ser distinto"},
    {"direct", "directa"},
    {"distant", "distante"},
    {"do", "hacer"},
    {"does", "hace"},
    {"dog", "perro"},
    {"do not", "no"},
    {"done", "hecho"},
    {"door", "puerta"},
    {"down", "abajo"},
    {"draw", "dibujar"},
    {"drive", "unidad"},
    {"dry", "seco"},
    {"during", "durante"},
    {"each", "cada"},
    {"early", "temprana"},
    {"earth", "tierra"},
    {"ease", "aliviar"},
    {"east", "al este"},
    {"eat", "comer"},
    {"egg", "huevo"},
    {"end", "fin"},
    {"energy", "energia"},
    {"enough", "suficiente"},
    {"equate", "equiparar"},
    {"even", "incluso"},
    {"ever", "nunca"},
    {"every", "cada"},
    {"example", "ejemplo"},
    {"eye", "ojo"},
    {"face", "cara"},
    {"fact", "hecho"},
    {"fall", "caer"},
    {"family", "familia"},
    {"far", "ahora"},
    {"farm", "granja"},
    {"fast", "rApido"},               // A
    {"father", "padre"},
    {"feel", "sentir"},
    {"feet", "pies"},
    {"few", "pocos"},
    {"field", "campo"},
    {"figure", "figura"},
    {"fill", "llenar"},
    {"final", "Ultimo"},             // U
    {"find", "encontrar"},
    {"fine", "fina"},
    {"fire", "fuego"},
    {"first", "primero"},
    {"fish", "peces"},
    {"five", "cinco"},
    {"fly", "volar"},
    {"follow", "siga"},
    {"food", "alimentos"},
    {"foot", "pie"},
    {"for", "para"},
    {"force", "fuerza"},
    {"form", "forma"},
    {"found", "encontrado"},
    {"four", "cuatro"},
    {"free", "libre"},
    {"friend", "amigo"},
    {"from", "desde"},
    {"front", "delante"},
    {"full", "completo"},
    {"game", "juego"},
    {"gave", "dio"},
    {"get", "conseguir"},
    {"girl", "chica"},
    {"give", "dar"},
    {"go", "ir"},
    {"gold", "oro"},
    {"good", "buena"},
    {"got", "conseguido"},
    {"govern", "gobernar"},
    {"great", "gran"},
    {"green", "verde"},
    {"ground", "suelo"},
    {"group", "grupo"},
    {"grow", "crecer"},
    {"had", "tenido"},
    {"half", "medio"},
    {"hand", "mano"},
    {"happen", "suceder"},
    {"hard", "duro"},
    {"has", "tiene"},
    {"have", "tener"},
    {"he", "El"},
    {"head", "cabeza"},
    {"hear", "escuchar"},
    {"heard", "oido"},
    {"heat", "calor"},
    {"help", "ayudar"},
    {"her", "su"},
    {"here", "aqui"},
    {"high", "alto"},
    {"him", "El"},
    {"his", "su"},
    {"hold", "mantenga"},
    {"home", "casa"},
    {"horse", "caballo"},
    {"hot", "caliente"},
    {"hour", "horas"},
    {"house", "casa"},
    {"how", "cOmo"},                    // O
    {"hundred", "cien"},
    {"hunt", "caza"},
    {"i", "yo"},
    {"idea", "gusta"},
    {"if", "si"},
    {"in", "en"},
    {"inch", "pulgadas"},
    {"interest", "interEs"},            //  E
    {"is", "es"},
    {"island", "isla"},
    {"it", "lo"},
    {"just", "sOlo"},
    {"keep", "mantener"},
    {"kind", "tipo"},
    {"king", "rey"},
    {"knew", "sabia"},
    {"know", "saber"},
    {"land", "tierra"},
    {"language", "idioma"},
    {"large", "grande"},
    {"last", "Ultimo"},                // U
    {"late", "tarde"},
    {"laugh", "risa"},
    {"lay", "sentar"},
    {"lead", "conducir"},
    {"learn", "aprender"},
    {"leave", "dejar"},
    {"left", "izquierda"},
    {"less", "menos"},
    {"let", "dejar"},
    {"letter", "carta"},
    {"life", "vida"},
    {"light", "luz"},
    {"like", "como"},
    {"line", "linea"},
    {"list", "lista"},
    {"listen", "escuchar"},
    {"little", "poco"},
    {"live", "vivir"},
    {"long", "largo"},
    {"look", "buscar"},
    {"love", "amar"},
    {"low", "bajo"},
    {"machine", "mAquina"},     //  A
    {"made", "hecho"},
    {"main", "principal"},
    {"make", "hacer"},
    {"man", "hombre"},
    {"many", "muchos"},
    {"map", "mapa"},
    {"mark", "marca"},
    {"may", "puede"},
    {"me", "me"},
    {"mean", "significarA"},        //  A
    {"measure", "medir"},
    {"men", "hombres"},
    {"might", "podria"},
    {"mile", "milla"},
    {"mind", "mente"},
    {"minute", "minuto"},
    {"miss", "seNorita"},
    {"money", "dinero"},
    {"moon", "luna"},
    {"more", "mAs"},            // A
    {"morning", "maNana"},      //  N
    {"most", "mAs"},            // A
    {"mother", "madre"},
    {"mountain", "montaNa"},    // N
    {"move", "movimiento"},
    {"much", "mucho"},
    {"multiply", "multiplicar"},
    {"music", "mUsica"},        // U
    {"must", "debe"},
    {"my", "mi"},
    {"name", "nombre"},
    {"near", "cerca"},
    {"need", "necesitarA"},
    {"never", "nunca"},
    {"new", "nuevo"},
    {"next", "prOximo"},         // O
    {"night", "noche"},
    {"no", "no"},
    {"north", "norte"},
    {"note", "nota"},
    {"nothing", "nada"},
    {"notice", "aviso"},
    {"noun", "sustantivo"},
    {"now", "ahora"},
    {"number", "nUmero"},       // U
    {"numeral", "numeral"},
    {"object", "objeto"},
    {"ocean", "ocEano"},         // E
    {"of", "de"},
    {"off", "fuera"},
    {"often", "menudo"},
    {"oh", "oh"},
    {"old", "viejo"},
    {"on", "en"},
    {"once", "una vez"},
    {"one", "uno"},
    {"only", "sOlo"},
    {"open", "abierta"},
    {"or", "o"},
    {"order", "orden"},
    {"other", "otros"},
    {"our", "nuestro"},
    {"out", "fuera"},
    {"over", "sobre"},
    {"own", "propio"},
    {"page", "pAgina"},    // A
    {"paint", "pintar"},
    {"paper", "papel"},
    {"part", "parte"},
    {"pass", "pasar"},
    {"pattern", "patrOn"},    // O
    {"people", "personas"},
    {"person", "persona"},
    {"picture", "imagen"},
    {"piece", "pieza"},
    {"place", "lugar"},
    {"plain", "llano"},
    {"plan", "plan de"},
    {"plane", "plano"},
    {"plant", "planta"},
    {"play", "jugar"},
    {"point", "punto"},
    {"port", "puerto"},
    {"pose", "plantear"},
    {"possible", "posible"},
    {"pound", "libra"},
    {"power", "potencia"},
    {"press", "prensa"},
    {"probable", "probable"},
    {"problem", "problema"},
    {"produce", "produce"},
    {"product", "producto"},
    {"pull", "halar"},
    {"put", "poner"},
    {"question", "pregunta"},
    {"quick", "rApido"},            // A
    {"rain", "lluvia"},
    {"ran", "corriO"},              // O
    {"reach", "llegar"},
    {"read", "leer"},
    {"ready", "listo"},
    {"real", "reales"},
    {"record", "registro"},
    {"red", "rojo"},
    {"remember", "recordar"},
    {"rest", "resto"},
    {"right", "derecho"},
    {"river", "rio"},
    {"road", "carretera"},
    {"rock", "roca"},
    {"room", "habitaciOn"},         // O
    {"round", "ronda"},
    {"rule", "regla"},
    {"run", "ejecutar"},
    {"said", "dicho"},
    {"same", "misma"},
    {"saw", "sierra"},
    {"say", "decir"},
    {"school", "escuela"},
    {"science", "ciencia"},
    {"sea", "mar"},
    {"second", "segundo"},
    {"see", "ver"},
    {"seem", "parecer"},
    {"self", "auto"},
    {"sentence", "frase"},
    {"serve", "servir"},
    {"set", "conjunto"},
    {"several", "varios"},
    {"shape", "forma"},
    {"she", "ella"},
    {"ship", "buque"},
    {"short", "corto"},
    {"should", "deberia"},
    {"show", "show"},
    {"side", "lado"},
    {"simple", "sencilla"},
    {"since", "desde"},
    {"sing", "cantar"},
    {"six", "seis"},
    {"slow", "lenta"},
    {"small", "pequeNo"},          // N
    {"snow", "nieve"},
    {"so", "asi"},
    {"some", "algunos"},
    {"song", "canciOn"},
    {"soon", "pronto"},
    {"sound", "sonar"},
    {"south", "sur"},
    {"space", "espacio"},
    {"special", "especial"},
    {"spell", "deletrear"},
    {"stand", "de pie"},
    {"star", "estrella"},
    {"start", "inicio"},
    {"state", "estado"},
    {"stay", "quedarse"},
    {"stead", "lugar"},
    {"step", "paso"},
    {"still", "todavia"},
    {"stood", "destacado"},
    {"stop", "detEngase"},      // E
    {"story", "historia"},
    {"street", "calle"},
    {"strong", "fuerte"},
    {"study", "estudio"},
    {"such", "tal"},
    {"sun", "sol"},
    {"sure", "seguro"},
    {"surface", "superficie"},
    {"system", "sistema"},
    {"table", "mesa"},
    {"tail", "cola"},
    {"take", "tomar"},
    {"talk", "charla"},
    {"teach", "enseNar"},
    {"tell", "decir"},
    {"ten", "diez"},
    {"test", "prueba"},
    {"than", "que"},
    {"that", "que"},
    {"the", "the"},
    {"their", "su"},
    {"them", "ellos"},
    {"then", "entonces"},
    {"there", "hay"},
    {"these", "Estos"},        // E
    {"they", "ellos"},
    {"thing", "cosa"},
    {"think", "pensar"},
    {"this", "este"},
    {"those", "los"},
    {"though", "aunque"},
    {"thought", "pensado"},
    {"thousand", "mil"},
    {"three", "tres"},
    {"through", "a travEs de"},      // E
    {"time", "tiempo"},
    {"tire", "neumAticos"},          // A
    {"to", "a"},
    {"together", "juntos"},
    {"told", "dicho"},
    {"too", "demasiado"},
    {"took", "tomO"},                // O
    {"top", "cima"},
    {"toward", "hacia"},
    {"town", "ciudad"},
    {"travel", "viajes"},
    {"tree", "Arbol"},               // A
    {"try", "tratar"},
    {"turn", "a su vez"},
    {"two", "dos"},
    {"under", "bajo"},
    {"unit", "unidad"},
    {"until", "hasta"},
    {"up", "hasta"},
    {"us", "nosotros"},
    {"use", "uso"},
    {"usual", "habitual"},
    {"verb", "verbo"},
    {"very", "muy"},
    {"voice", "voz"},
    {"vowel", "vocal"},
    {"wait", "espere"},
    {"walk", "caminar"},
    {"want", "querer"},
    {"war", "guerra"},
    {"warm", "caliente"},
    {"was", "era"},
    {"watch", "ver"},
    {"water", "agua"},
    {"way", "camino"},
    {"we", "nos"},
    {"week", "semana"},
    {"well", "asi"},
    {"went", "se fue"},
    {"were", "eran"},
    {"west", "oeste"},
    {"what", "quE"},     // E
    {"wheel", "rueda"},
    {"when", "cuando"},
    {"where", "donde"},
    {"which", "que"},
    {"while", "mientras"},
    {"white", "blanco"},
    {"who", "que"},
    {"whole", "todo"},
    {"why", "por quE"},       // E
    {"will", "lo harA"},      // A
    {"wind", "viento"},
    {"with", "con"},
    {"wonder", "maravilla"},
    {"wood", "madera"},
    {"word", "palabra"},
    {"work", "trabajo"},
    {"world", "mundo"},
    {"would", "haria"},
    {"write", "escribir"},
    {"year", "aNos"},
    {"yes", "si"},
    {"you", "usted"},
    {"young", "joven"},
    {"your", "su"}
};

// Init
void intr_reg_handler();
void intr_init();
void intr_start();
void intr_enable();
void intr_disable();
static inline unsigned char inb ();
static inline void outb ();
static inline void outw();
void keyb_init();
void keyb_handler();
void keyb_process_keys();
void cursor_moveto();
void out_str();

int strcmp();
void itoa(int numb);
void parse_cmd();

void backspace_key();
void enter_key();
void command_handler();
void info_command();
void dictinfo_command();
void translate_command();
void wordstat_command();
void init_lcg();
void anyword_command();
void shutdown_command();
void unknown_command();
void clear();
void boot_input();
void input_init();

char scan_code_to_symbol[] = {
    0, 0,
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    0, 0, 0, 0,
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
    0, 0, 0, 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
    0, 0, 0, 0, 0,
    'z', 'x', 'c', 'v', 'b', 'n', 'm',
    0, 0,
    '/',
    0,
    '*',
    0,
    ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-',
    0, 0, 0,
    '+'
};

char scan_code_to_symbol_shift[] = {
	0, 0,
	'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	0, 0, 0, 0,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	0, 0, 0, 0,
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
	0, 0, 0, 0, 0,
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	0, 0,
	'/',
	0,
	'*',
	0,
	' ',
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	'_',
	0, 0, 0,
	'+'
};

// Структура описывает данные об обработчикепрерывания
struct idt_entry
{
    unsigned short base_lo;
    unsigned short segm_sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;  
}__attribute__((packed)); // Выравнивание запрещено

// Структура, адрес которой передается как аргумент команды lidt
struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
}__attribute__((packed)); // Выравнивание запрещено

struct idt_entry g_idt[256]; // Реальная таблица IDT
struct idt_ptr g_idtp;       // Описатель таблицы для команды lidt

// Пустой обработчик прерываний. Другие обработчики могут быть реализованы по этому шаблону
void default_intr_handler()
{
    asm("pusha");
    // ... (реализация обработки)
    asm("popa; leave; iret");
}

typedef void (*intr_handler)();

void intr_reg_handler(int num, unsigned short segm_sel, unsigned short flags, intr_handler hndlr)
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
    {
        intr_reg_handler(i, GDT_CS, 0x80 | IDT_TYPE_INTR,default_intr_handler); // segm_sel=0x8, P=1, DPL=0, Type=Intr
    }
}

void intr_start()
{
    int idt_count = sizeof(g_idt) / sizeof(g_idt[0]);

    g_idtp.base = (unsigned int) (&g_idt[0]);
    g_idtp.limit = (sizeof (struct idt_entry) * idt_count) -1;

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

static inline unsigned char inb (unsigned short port)
{
    unsigned char data;
    asm volatile ("inb %w1, %b0" : "=a" (data) : "Nd" (port));
    return data;
}

static inline void outb (unsigned short port, unsigned char data)
{
    asm volatile ("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

static inline void outw(unsigned int port, unsigned int data)
{
	asm volatile ("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

void keyb_init()
{
    intr_reg_handler(0x09, GDT_CS, 0x80 | IDT_TYPE_INTR, keyb_handler);
    outb(PIC1_PORT+ 1, 0xFF ^ 0x02);
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

void cursor_moveto(unsigned int strnum, unsigned int pos)
{
    unsigned short new_pos = (strnum * VIDEO_WIDTH) + pos;
    outb(CURSOR_PORT, 0x0F);
    outb(CURSOR_PORT + 1, (unsigned char)(new_pos & 0xFF));
    outb(CURSOR_PORT, 0x0E);
    outb(CURSOR_PORT + 1, (unsigned char)( (new_pos >> 8) & 0xFF));
}

int unix_scroll(int count) {
	unsigned char* video_buf = (unsigned char*)VIDEO_BUF_PTR;
	int lines = current_strnum + count - VIDEO_HEIGHT + 1;
	for (int l = 0; l < lines; l++) 
	{
		for (int i = 0; i < VIDEO_WIDTH * 2 * (VIDEO_HEIGHT - 1); i++)
			video_buf[i] = video_buf[i + VIDEO_WIDTH * 2];

		for (int i = 0; i < VIDEO_WIDTH * 2; i++) 
			video_buf[VIDEO_WIDTH * 2 * (VIDEO_HEIGHT - 1) + i] = 0;
	}
	return lines;
}

void out_str(int color, const char* ptr, unsigned int strnum)
{
    unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
    video_buf += 2 * (VIDEO_WIDTH * current_strnum + current_pos);

    while (*ptr)
    {
        video_buf[0] = (unsigned char) *ptr;
        video_buf[1] = color;
        video_buf += 2;
        ptr++;
    }
}

void out_translate(int color, const char* ptr, unsigned int strnum)
{
    unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
    video_buf += 2 * (VIDEO_WIDTH * current_strnum + current_pos);

    while (*ptr)
    {
        if(*ptr == 'N')
        {
            for_lcg += 89;
            unsigned char n = 164;
            unsigned char* later_n = &n;
            video_buf[0] = (unsigned char) *later_n;
            video_buf[1] = color;
            video_buf += 2;
            ptr++;
        }
        else if(*ptr == 'O')
        {
            for_lcg += 78;
            unsigned char n = 162;
            unsigned char* later_n = &n;
            video_buf[0] = (unsigned char) *later_n;
            video_buf[1] = color;
            video_buf += 2;
            ptr++;
        }
        else if(*ptr == 'E')
        {
            for_lcg += 67;
            unsigned char n = 130;
            unsigned char* later_n = &n;
            video_buf[0] = (unsigned char) *later_n;
            video_buf[1] = color;
            video_buf += 2;
            ptr++;
        }
        else if(*ptr == 'A')
        {
            for_lcg += 56;
            unsigned char n = 160;
            unsigned char* later_n = &n;
            video_buf[0] = (unsigned char) *later_n;
            video_buf[1] = color;
            video_buf += 2;
            ptr++;
        }
        else if(*ptr == 'U')
        {
            for_lcg += 45;
            unsigned char n = 163;
            unsigned char* later_n = &n;
            video_buf[0] = (unsigned char) *later_n;
            video_buf[1] = color;
            video_buf += 2;
            ptr++;
        }
        else
        {
            for_lcg += 14;
            video_buf[0] = (unsigned char) *ptr;
            video_buf[1] = color;
            video_buf += 2;
            ptr++;
        }
    }
}

int strcmp(const char* str1, const char* str2)
{
    for_lcg += 234;
    while (*str1 == *str2 && *str1 != '\0' && *str2 != '\0')
    {
        str1++;
        str2++;
    }
    if (*str1 == '\0' && *str2 == '\0')
        return 0;
    else if(*str1 < *str2)
        return 1;
    return -1;
}

bool check_if_no_words(char char_to_find)
{
    for_lcg += 689;
    if(start_point[char_to_find - 'a'] == -1 || 
        start_point[char_to_find - 'a'] == start_point[char_to_find - 'a' + 1] ||
        start_point[char_to_find - 'a' + 1] == -1)
    {
        return true;
    }
    return false;
}

void itoa(int numb)
{
    for_lcg += 37;
    char tmpbuf[40];
    for(int i = 0; i < 40; i++)
    {
        buffer[i] = '\0';
        tmpbuf[i] = '\0';
    }

    if(numb != 0)
    {
        for_lcg += 56;
        int i = 0, j = 0;
        while(numb > 0)
        {
            tmpbuf[i] = numb % 10 + '0';
            numb /= 10;
            i++;
        }
        i--;
        while(i >= 0)
        {
            buffer[j] = tmpbuf[i];
            j++;
            i--;
        }
    }
    else
        buffer[0] = '0';
    
}

void parse_cmd()
{
    int i = 0;
    strlen_arg = 0;
    while(i < current_pos && usrinput[i] != ' ')
    {
        for_lcg += 2;
        cmd_command[i] = usrinput[i];
        i++;
    }   
    i++;
    while(i < current_pos && usrinput[i] != ' ')
    {
        for_lcg += 51;
        cmd_arg[strlen_arg] = usrinput[i];
        i++;
        strlen_arg++;
    }
}

void keyb_process_keys()
{
    // Проверка что буфер PS/2 клавиатуры не пуст (младший бит присутствует)
    if (inb(0x64) & 0x01)
    {
        scan_code = inb(0x60); // Считывание символа с PS/2 клавиатуры
        if (scan_code < 128) // Скан-коды выше 128 - это отпускание клавиши
        {
            if (scan_code == 14)
                backspace_key();
            else if (scan_code == 28)
                enter_key();
            else if (scan_code == 42 || scan_code == 54)
		        shift = 1;
            else if (current_pos - 8 < MAX_COMMAND_LENGTH){
                if (shift == 0){
                    char c = scan_code_to_symbol[scan_code];
                    if (c == 0)
                        return;
                    usrinput[count_usrinput] = c;
                    count_usrinput++;
                    out_str(USR_COLOR, &c, current_strnum);
                    current_pos++;
                    cursor_moveto(current_strnum, current_pos);
                }
                else if (shift == 1){
                    char c = scan_code_to_symbol_shift[scan_code];
                    if (c == 0)
                        return;
                    usrinput[count_usrinput] = c;
                    count_usrinput++;
                    out_str(USR_COLOR, &c, current_strnum);
                    current_pos++;
                    cursor_moveto(current_strnum, current_pos);
                }
            }
        }
        else if (scan_code == 170 || scan_code == 182) {
			shift = 0;
        }
    }
}

void backspace_key()
{
    for_lcg += 83;
    if (current_pos > 8)
    {   
        count_usrinput--;
        usrinput[count_usrinput] = '\0';
        current_pos--;
        unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
        video_buf += 2 * (current_strnum * VIDEO_WIDTH + current_pos);
        video_buf[0] = 0;
        cursor_moveto(current_strnum, current_pos);
    }
}

void enter_key()
{
    for_lcg += 615;
    command_handler();
    input_init();
}

void command_handler()
{
    for_lcg += 1;
    unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
    video_buf += 2 * (current_strnum * VIDEO_WIDTH + 8);
    parse_cmd();
    current_pos = 0;

    if (strcmp(cmd_command, "info\0") == 0){
        if (current_strnum + 4 >= VIDEO_HEIGHT)
	    {
		    int str_scrolled = unix_scroll(4);
		    current_strnum -= str_scrolled;
	    }
        info_command();
    }
    else if(strcmp(cmd_command, "dictinfo\0") == 0){
        if (current_strnum + 4 >= VIDEO_HEIGHT)
	    {
		    int str_scrolled = unix_scroll(4);
		    current_strnum -= str_scrolled;
	    }
        dictinfo_command();
    }
    else if(strcmp(cmd_command, "translate\0") == 0){
        if (current_strnum + 2 >= VIDEO_HEIGHT)
	    {
		    int str_scrolled = unix_scroll(2);
		    current_strnum -= str_scrolled;
	    }
        translate_command();
    }
    else if(strcmp(cmd_command, "wordstat\0") == 0){
        if (current_strnum + 2 >= VIDEO_HEIGHT)
	    {
		    int str_scrolled = unix_scroll(2);
		    current_strnum -= str_scrolled;
	    }
        wordstat_command();
    }
    else if(strcmp(cmd_command, "anyword\0") == 0){
        if (current_strnum + 2 >= VIDEO_HEIGHT)
	    {
		    int str_scrolled = unix_scroll(2);
		    current_strnum -= str_scrolled;
	    }
        anyword_command();
    }
    else if(strcmp(cmd_command, "clear\0") == 0)
        clear();
    else if (strcmp(cmd_command, "shutdown\0") == 0){
        if (current_strnum + 2 >= VIDEO_HEIGHT)
	    {
		    int str_scrolled = unix_scroll(2);
		    current_strnum -= str_scrolled;
	    }
        shutdown_command();
    }
    else{
        if (current_strnum + 2 >= VIDEO_HEIGHT)
	    {
		    int str_scrolled = unix_scroll(2);
		    current_strnum -= str_scrolled;
	    }
        unknown_command();
    }
}

void info_command()
{
    for_lcg += 58;

    out_info = dictionary;
    out_str(COLOR, "DictOS. Developer: Egor Chuprov, 4831001/10002, SpbPU, 2023", current_strnum++);
    out_str(COLOR, "Compilers: bootloader: yasm, kernel: gcc", current_strnum++);
    out_str(COLOR, "Bootloader parameter: ", current_strnum++);
    current_pos = 22;
    out_str(COLOR, out_info, current_strnum);
    current_strnum++;
}

void dictinfo_command()
{
    for_lcg += 34;
    out_str(COLOR, "Dictionary: en -> es", current_strnum++);
    out_str(COLOR, "Number of words: ", current_strnum++);
    current_pos = 17;
    itoa(504);
    out_info = buffer;
    out_str(COLOR, out_info, current_strnum);
    current_pos = 0;
    out_str(COLOR, "Number of loaded words: ", current_strnum++);
    current_pos = 24;
    itoa(chosen_words);
    out_info = buffer;
    out_str(COLOR, out_info, current_strnum);
    current_strnum++;
}

int find_str_dict(const char* to_find)
{
    for_lcg += 1;
    int l, r, mid, result = -1;
    l = start_point[to_find[0] - 'a'];
    r = start_point[to_find[0] - 'a' + 1];

    if(!check_if_no_words(to_find[0]))
    {
        mid =  l / 2 + r / 2;
        while(strcmp(dict[l][0], to_find) != 0)
        {
            if(r <= l)
            {
                result = -1;
                return result;
            }
            else if(strcmp(dict[mid][0], to_find) == 1)
                l = mid + 1;
            else
                r = mid;
            mid = l / 2 + r / 2;
        }
        result = l;
    }
    return result;
}

void translate_command()
{
    for_lcg += 23;
    int strind = find_str_dict(cmd_arg);
    if(strind == -1)
    {
        out_str(COLOR, "Such word doesnt exist in dictionary", current_strnum++);
    }
    else
    {
        int i = 0;
        while(dict[strind][1][i] != '\0')
        {
            tmp[i] = dict[strind][1][i];
            i++;
        }
        out_info = tmp;
        out_translate(COLOR, out_info, current_strnum++);
    }
    current_strnum++;
}

void wordstat_command()
{
    for_lcg += 9;
    if(cmd_arg[0] == '\0')
    {
        out_str(COLOR, "Error: no argument", current_strnum++);
        current_strnum++;
        return;
    }
    else if(cmd_arg[1] != '\0')
    {
        out_str(COLOR, "Error: many arguments", current_strnum++);
        current_strnum++;
        return;
    }

    int words_number = 0;

    char char_to_find = cmd_arg[0];
 
    if(!check_if_no_words(char_to_find))
        words_number = start_point[char_to_find - 'a' + 1] - start_point[char_to_find - 'a'];
    
    out_str(COLOR, "Chosen words: ", current_strnum++);
    itoa(words_number);
    current_pos = 14;
    out_info = buffer;
    out_str(COLOR, out_info, current_strnum);
    current_strnum++;
}

void anyword_command()
{
    x_lcg += for_lcg;
	unsigned int m_lcg = 2147483648;
	unsigned int c_lcg = 2531011;
    unsigned int a_lcg = 214013;

    unsigned int lett_index, word_index, rand_ind;
    
    if(cmd_arg[1] != '\0')
    {
        out_str(COLOR, "Error: many arguments", current_strnum++);
        current_strnum++;
        return;
    }
    else if(cmd_arg[1] == '\0')
    {
        if(cmd_arg[0] == '\0')
        {
            int ceil = boot_input_length;
            int result = 0;

            x_lcg = (a_lcg * x_lcg + c_lcg) % m_lcg;

            if(ceil > 0)
                result = x_lcg % ceil;

            lett_index = dictionary[result] - 'a';
            int start = start_point[lett_index];
            int finish = start_point[lett_index + 1];
            ceil = finish - start;
            result = 0;

            x_lcg = (a_lcg * x_lcg + c_lcg) % m_lcg;

            if(ceil > 0)
                result = x_lcg % ceil;

            rand_ind = result;

        }
        else
        {
            lett_index = cmd_arg[0] - 'a';
            int checker = 1;

            if(check_if_no_words(cmd_arg[0]))
            {   
                out_str(COLOR, "Error: no words", current_strnum++);
                current_strnum++;
                return;
            }

            int start = start_point[lett_index];
            int finish = start_point[lett_index + 1];
            int ceil = finish - start;
            int result = 0;

            x_lcg = (a_lcg * x_lcg + c_lcg) % m_lcg;

            if(ceil > 0)
                result = x_lcg % ceil;

            rand_ind = result;
        }
        unsigned int word_index = start_point[lett_index] + rand_ind;
        int i = 0;
        while(dict[word_index][0][i] != '\0')
        {
            tmp[i] = dict[word_index][0][i];
            i++;
        }
        out_info = tmp;
        out_translate(COLOR, out_info, current_strnum++);
        current_pos += i;
        out_str(COLOR, ": ", current_strnum);
        current_pos += 2;

        i = 0;
        while(dict[word_index][1][i] != '\0')
        {
            tmp[i] = dict[word_index][1][i];
            i++;
        }
        out_info = tmp;
        out_translate(COLOR, out_info, current_strnum);
    }
    current_strnum++;
}

void shutdown_command()
{
    outw(0x604, 0x2000);
}

void unknown_command()
{
    for_lcg += 47;
    out_str(COLOR, "Error: command not recognized", current_strnum++);
    current_strnum++;
}

void clear()
{
    for_lcg += 1;
    unsigned char* video_buf = (unsigned char*) VIDEO_BUF_PTR;
    for (int i = 0; i <= VIDEO_WIDTH * VIDEO_HEIGHT * 2; i++)
        video_buf[i] = 0;
    current_strnum = COMMAND_START_STRNUM;
}

void boot_input()
{
    char *ptr = (char *)0x9000;
    
    for(int i = 0; i < 26; i++)
    {
        if(*ptr != '_')
        {
            dictionary[boot_input_length] = *ptr;
            boot_input_length++;
        }
        ptr++;
    }
    dictionary[boot_input_length] = '\0';
}

void input_init()
{
    for(int i = 0; i < 40; i++)
    {
        for_lcg += 1;
        usrinput[i] = '\0';
        cmd_command[i] = '\0';
        cmd_arg[i] = '\0';
        tmp[i] = '\0';
    }
    count_usrinput = 0;

    const char* input_command = "DictOS# ";
    current_pos = 0;
    out_str(0x02, input_command, current_strnum);
    current_pos = COMMAND_START_POS;
    cursor_moveto(current_strnum, current_pos);
}

void init_dict()
{
    for(int i = 0; i < 27; i++)
        start_point[i] = -1;

    for(int i = 0; i < boot_input_length; i++)
    {
        for_lcg += 1;
        int l, r, mid, to_find;
        l = 0;
        r = TOTAL_WORDS;
        mid =  l / 2 + r / 2;
        to_find = dictionary[i];

        while(dict[l][0][0] != to_find)
        {
            for_lcg += 1;
            if(r <= l)
            {
                l = -1;
                break;
            }
            if(dict[mid][0][0] < to_find)
                l = mid + 1;
            else
                r = mid;
            mid = l / 2 + r / 2;
        }
        
        if(l != -1)
        {
            for_lcg += 1;
            while(l > 0 && dict[l-1][0][0] == to_find)
                l--;
            start_point[to_find - 'a'] = l;

            while(l < TOTAL_WORDS - 1 && dict[l+1][0][0] == to_find)
                l++;
            start_point[to_find - 'a' + 1] = l + 1;
            chosen_words += start_point[to_find - 'a' + 1] - start_point[to_find - 'a'];
        }
    }
}

extern "C" int kmain()
{
    intr_disable();
	intr_init();
    boot_input();
    init_dict();

    keyb_init();
    input_init();

	intr_start();
	intr_enable();

    while(1)
    {
        asm("hlt");
    }

    return 0;
}
