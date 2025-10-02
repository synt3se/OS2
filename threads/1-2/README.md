gcc -o a -Wall -Wextra -pedantic a.c -lpthread

gcc -o b -Wall -Wextra -pedantic b.c -lpthread

gcc -o c -Wall -Wextra -pedantic c.c -lpthread

gcc -o d -Wall -Wextra -pedantic d.c -lpthread

gcc -o e -Wall -Wextra -pedantic e.c -lpthread

gcc -o e -Wall -Wextra -pedantic e.c -lpthread

watch cat /proc//maps
cat /proc/sys/vm/max_map_count - ограничение на количество mmap() областей
Узнать количество мапов для процесса: cat /proc/PID/maps | wc -l

d - main: threads created 524277

cat /proc/sys/kernel/threads-max
cat /proc/sys/kernel/pid_max - количество tid
