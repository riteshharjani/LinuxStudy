## Reading All the Bytes

*This snippet handles all five conditions*

```
ssize_t ret;
while (len != 0 && (ret = read (fd, buf, len)) != 0) {
  if (ret == −1) {
    if (errno == EINTR)
      continue;
    perror ("read");
    break;
  }
  len -= ret;
  buf += ret;
}
```
* The loop reads len bytes from the current file position of fd into buf, which of course must be at least len bytes in length. 
* It continues reading until it reads all len bytes, or until EOF is reached. 
* If more than zero, but less than len bytes are read, len is reduced by the amount read, buf is increased by the
amount read, and the call is reissued. 
* If the call returns −1, and errno equals EINTR, the call is reissued without updating the parameters. 
* If the call returns −1, and errno is set to anything else, perror() is called to print a description to standard error and the loop
terminates.
