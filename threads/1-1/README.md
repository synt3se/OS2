gcc -o a -Wall -Wextra -pedantic a.c -lpthread

gcc -o b -Wall -Wextra -pedantic b.c -lpthread

gcc -o c -Wall -Wextra -pedantic c.c -lpthread

gcc -o d -Wall -Wextra -pedantic d.c -lpthread

cat /proc/pid/maps

strace ./c

