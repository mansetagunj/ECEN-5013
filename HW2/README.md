

strace command:
strace -C -r -tt -T -o strace_prob2_log ./prob2.out


ltrace command:
ltrace -n4 -T -r -S -o ltrace_prob2_log ./prob2.out
