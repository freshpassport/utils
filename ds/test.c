#include "list.h"

struct test {
        struct list list;
        int a, b, c;
};

void free_list(struct list *list)
{
        struct list *n, *nt;
        struct test *elem;

        if (!list_empty(list))
        {
                list_iterate_safe(n, nt, list)
                {
                        elem = list_struct_base(n, struct test, list);
                        printf("free %.8p\n", elem);
                        free(elem);
                }
        }
}

int main()
{
        int i;
        struct test *t;
        struct list *p, *pt;
        LIST_INIT(list);

        for (i=0; i<10; i++)
        {
                t = (struct test*)malloc(sizeof(struct test));
                t->a = t->b = t->c = (i+1);
                list_add(&list, &t->list);
                printf("add %.8p\n", t);
        }

        if (!list_empty(&list))
        {
                printf("list size = %d\n", list_size(&list));

                list_iterate_safe(p, pt, &list)
                {
                        t = list_struct_base(p, struct test, list);
                        printf("t->a = %d\n", t->a);
                }
        }

        free_list(&list);
}
