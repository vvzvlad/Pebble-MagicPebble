#include "pebble.h"

float timer_delay=1; /* Создаем переменную для с временем для таймера */
Window *window; /* Создаем указатель на окно */
TextLayer *text_layer;  /* создаем  указатель на текстовый слой */
bool first_time=true; /* создаем флаг первого запуска */
int lang=0; /* создаем переменную языка: eng-0, rus-1, ge-2 */

static const char* messages[2][21] =
{
    {"It is certain", "It is decidedly so", "Without a doubt", "Yes — definitely", "You may rely on it", "As I see it, yes", "Most likely", "Outlook good", "Signs point to yes", "Yes", "Reply hazy, try again", "Ask again later", "Better not tell you now", "Cannot predict now", "Concentrate and ask again", "Don’t count on it", "My reply is no", "My sources say no", "Outlook not so good ", "Very doubtful",},
    {"Бесспорно", "Это предрешено","Никаких сомнений","Определенно - да","Будь уверен в этом","Мне кажется - да","Вероятнее всего","Хорошие перспективы","Да","Знаки говорят - да","Пока не ясно, попробуй еще раз","Спроси позже","Лучше не рассказывать сейчас","Сейчас нельзя предсказать","Сконцентрируйся и спроси снова","Даже не думай","Мой ответ - нет","Знаки говорят - нет","Перспективы не очень хорошие","Весьма сомнительно","Нет",},
};

static const char* messages_hello[3] =
{
    "Magic Pebble \n \n Ask a question that you can answer \n \"yes\" or \"no\" and click button -->",
    "Magic Pebble \n Задай вопрос, на который можно ответить \"да\" или \"нет\" и нажми \n на кнопку -->",
    "Magic Pebble \n\nСlick any button or long press Select\nto switch lang: \n ENG/RUS",
};


void timer_call() /* эта функция вызывается при срабатываниии таймера */
{
    text_layer_set_text(text_layer, messages[lang][rand() % 20]); /* выводим случайное сообщение */
    if (timer_delay < 300*100 ) /* если задержка еще не достигла 300мс... */
    {
        timer_delay=timer_delay/0.7; /* ...увеличиваем задержку... */
        app_timer_register(timer_delay/100, timer_call, NULL); /* ...и взводим таймер заново */
    }
    else /* если задержка уже больше 300мс... */
    {
        timer_delay=1; /* сбрасываем таймер на начало и выходим - сообщение же уже вывели */
    }

}

void config_text_layer(int16_t x,int16_t y,int16_t h,int16_t w, const char *font_key)  /* для исключения дублирования кода, создали функцию, которая занимается инициализаций и настройкой текстового массива*/
{
    text_layer = text_layer_create(GRect(x, y, h, w)); /* создаем массив, указываем размер и координаты */
    text_layer_set_text_color(text_layer, GColorWhite);  /* устанавливаем цвет текста */
    text_layer_set_background_color(text_layer, GColorClear);  /* устанавливаем цвет фона */
    text_layer_set_font(text_layer, fonts_get_system_font(font_key)); /* устанавливаем шрифт */
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter); /* устанавливаем выравнивание по центру */
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));  /* подключаем текстовый слой к основному в качестве дочернего */
}

void click(ClickRecognizerRef recognizer, void *context)  /* функция, срабатывающая при клике на кнопки */
{
    text_layer_destroy(text_layer); /* очищаем и удаляем старый слой */
    config_text_layer(5, 40, 134, 120, FONT_KEY_GOTHIC_28); /* создаем новый слой с другими координатами и шрифтом */
    timer_call(); /* взводим таймер для быстрой смены сообщений */

}

void lang_change(ClickRecognizerRef recognizer, void *context)  /* функция изменения языка */
{
	lang++; /* при нажатии инкрементируем переменную языка */
	if (lang >= 2) /* при превышении ею количества языков... */
	{
		lang = 0; /* возвращаемся к первому */
	}
	click(NULL, NULL); /* вызываем генерацию картинки */
}

void WindowsClickConfigProvider(void *context)  /* функция, внутри которой должны находиться подписки на кнопки */
{
    window_single_click_subscribe(BUTTON_ID_UP, click); /* при нажатии на верхнюю кнопку запустить click */
    window_single_click_subscribe(BUTTON_ID_SELECT, click); 
    window_single_click_subscribe(BUTTON_ID_DOWN, click); 
    window_long_click_subscribe(BUTTON_ID_SELECT, 1000, lang_change, NULL); /* при долгом нажатии на кнопку "выбор" запустить lang_change - смену языка */

}

int main(void)
{
    window = window_create();  /* Инициализируем окно */
    window_set_background_color(window, GColorBlack); /* устанавливаем фоновый цвет */
    window_set_fullscreen(window, true); /* включаем полноэкранность */
    window_stack_push(window, true);  /* открываем окно */
    config_text_layer(0, 3, 144, 168, FONT_KEY_GOTHIC_24);
    srand(time(NULL)); /* инициализируем генератор случайных чисел текущем временем */
    window_set_click_config_provider(window, WindowsClickConfigProvider); /* определяем функцию, в которой будут находиться подписки на кнопки */
    if (persist_exists(1)) /* если в постоянной хранилице есть переменная языка... */
    {
        lang = persist_read_int(1); /* ...то читаем ее оттуда и записываем в локальную переменную */
        text_layer_set_text(text_layer, messages_hello[lang]);  /* показываем сообщение при запуске */
    }
    else /* если ее там нет... */
    {
       persist_write_int(1, lang); /* ... то значит записываем ее туда(первый запуск программы) */
       text_layer_set_text(text_layer, messages_hello[2]); /* и показываем сообщение для первого запуска */
    }

    app_event_loop();  /* ждем событий */
    persist_write_int(1, lang); /* при выходе записываем установленный в последний раз язык в постоянное хранилище */
    text_layer_destroy(text_layer); /* уничтожаем объекты, освобождаем ресурсы */
    window_destroy(window);  /* уничтожаем объекты, освобождаем ресурсы */
}
