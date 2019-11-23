#include <iostream>
#include <windows.h>

using namespace std;

/* структура, описывающая узел красно-черного дерева */
struct rb_node
{
    int red;
    int data;
    struct rb_node *link[2];
};

struct rb_tree
{
    struct rb_node *root;  // указатель на корневой узел
    int count;  // количество узлов в дереве
};

struct rb_tree * tree_create(void)
{
    struct rb_tree * new_tree;
    new_tree=new struct rb_tree;
    if (new_tree == nullptr) return nullptr;
    new_tree->root = nullptr;
    new_tree->count = 0;
    return new_tree;
}

int is_red ( struct rb_node *node )
{
    return node != nullptr && node->red == 1;
}

/* функция для однократного поворота узла */
struct rb_node *rb_single ( struct rb_node *root, int dir )
{
    struct rb_node *save = root->link[!dir];

    root->link[!dir] = save->link[dir];
    save->link[dir] = root;

    root->red = 1;
    save->red = 0;

    return save;
}

/* функция для двукратного поворота узла */
struct rb_node *rb_double ( struct rb_node *root, int dir )
{
    root->link[!dir] = rb_single ( root->link[!dir], !dir );
    return rb_single ( root, dir );
}

struct rb_node *make_node ( int data )
{
    struct rb_node *rn;
    rn=new struct rb_node;

    if ( rn != nullptr )
    {
        rn->data = data;
        rn->red = 1; /* –инициализация красным цветом */
        rn->link[0] = nullptr;
        rn->link[1] = nullptr;
    }
    return rn;
}

int rb_insert ( struct rb_tree *tree, int data )
{
    /* если добавляемый элемент оказывается первым – то ничего делать не нужно*/
    if ( tree->root == nullptr )
    {
        tree->root = make_node ( data );
        if ( tree->root == nullptr )
            return 0;
    }
    else
    {
        struct rb_node head = {0}; /* временный корень дерева*/
        struct rb_node *g, *t;     /* дедушка и родитель */
        struct rb_node *p, *q;     /* родитель и итератор */
        int dir = 0, last;

        /* вспомогательные переменные */
        t = &head;
        g = p = nullptr;
        q = t->link[1] = tree->root;

        /* основной цикл прохода по дереву */
        for ( ; ; )
        {
            if ( q == nullptr )
            {
                /* вставка ноды */
                p->link[dir] = q = make_node ( data );
                tree->count ++ ;
                if ( q == nullptr )
                    return 0;
            }
        else
            if ( is_red ( q->link[0] ) && is_red ( q->link[1] ) )
            {
                /* смена цвета */
                q->red = 1;
                q->link[0]->red = 0;
                q->link[1]->red = 0;
            }

        /* совпадение 2-х красных цветов */
        if ( is_red ( q ) && is_red ( p ) )
        {
            int dir2 = t->link[1] == g;

            if ( q == p->link[last])
                t->link[dir2] = rb_single ( g, !last );
            else
                t->link[dir2] = rb_double ( g, !last );
        }

        /* такой узел в дереве уже есть  - выход из функции*/
        if ( q->data == data )
        {
            break;
        }

        last = dir;
        dir = q->data < data;

        if ( g != nullptr )
            t = g;
        g = p;
        p = q;
        q = q->link[dir];
        }

        /* обновить указатель на корень дерева*/
        tree->root = head.link[1];
    }
    /* сделать корень дерева черным */
    tree->root->red = 0;

    return 1;
}

int br_remove ( struct rb_tree *tree, int data )
{
    if ( tree->root != nullptr )
    {
        struct rb_node head = {0}; /* временный указатель на корень дерева */
        struct rb_node *q, *p, *g; /* вспомогательные переменные */
        struct rb_node *f = nullptr;  /* узел, подлежащий удалению */
        int dir = 1;

        /* инициализация вспомогательных переменных */
        q = &head;
        g = p = nullptr;
        q->link[1] = tree->root;

        /* поиск и удаление */
        while ( q->link[dir] != nullptr )
        {
            int last = dir;

            /* сохранение иерархии узлов во временные переменные */
            g = p;
            p = q;
            q = q->link[dir];
            dir = q->data < data;

            /* сохранение найденного узла */
            if ( q->data == data )
            f = q;

            if ( !is_red ( q ) && !is_red ( q->link[dir] ) )
            {
                if ( is_red ( q->link[!dir] ) )
                    p = p->link[last] = rb_single ( q, dir );
                else
                    if ( !is_red ( q->link[!dir] ) )
                    {
                        struct rb_node *s = p->link[!last];

                        if ( s != nullptr )
                        {
                            if ( !is_red ( s->link[!last] ) && !is_red ( s->link[last] ) )
                            {
                                /* смена цвета узлов */
                                p->red = 0;
                                s->red = 1;
                                q->red = 1;
                            }
                            else
                            {
                                int dir2 = g->link[1] == p;

                                if ( is_red ( s->link[last] ) )
                                g->link[dir2] = rb_double ( p, last );
                                else if ( is_red ( s->link[!last] ) )
                                g->link[dir2] = rb_single ( p, last );

                                /* корректировка цвета узлов */
                                q->red = g->link[dir2]->red = 1;
                                g->link[dir2]->link[0]->red = 0;
                                g->link[dir2]->link[1]->red = 0;
                            }
                        }
                    }
            }
        }

        /* удаление найденного узла */
        if ( f != nullptr )
        {
            f->data = q->data;
            p->link[p->link[1] == q] =
            q->link[q->link[0] == nullptr];
            free ( q );
        }

        /* обновление указателя на корень дерева */
        tree->root = head.link[1];
        if ( tree->root != nullptr )
            tree->root->red = 0;
    }

return 1;
}

void print_sub(rb_node *q, long n) //вспомогательная рекурсивная функция для вывода дерева на экран
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    long i;
    if (q)
    {
        print_sub(q->link[1], n+5);
        for (i = 0; i < n; i++)
            printf(" ");
        if(q->red == 1) SetConsoleTextAttribute(hStdOut, FOREGROUND_RED);
        printf("%d", q->data);
        SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        printf("\n");
        print_sub(q->link[0], n+5);
    }
}

int main()
{
    struct rb_tree * my_tree=tree_create();
    int tmp;

    for(size_t i=0;i<100;i++)
    {
        tmp=rand();
        rb_insert(my_tree,tmp);
    }

    print_sub(my_tree->root, 0);

    return 0 ;
}
