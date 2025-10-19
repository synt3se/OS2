gcc -o a -Wall -Wextra -pedantic a.c -lpthread

gcc -o b -Wall -Wextra -pedantic b.c -lpthread

gcc -o b-2 -Wall -Wextra -pedantic c.c -lpthread

gcc -o b-bad -Wall -Wextra -pedantic c.c -lpthread

gcc -o c -Wall -Wextra -pedantic c.c -lpthread