# GNU-MAKE READER

## Description
	
Tool for reading makefiles and extracting targets and files involved. Useful if you want to implement makefile support to your IDE.

## Dependencies:

glib & makeheaders (http://www.hwaci.com/sw/mkhdr/) or (https://aur.archlinux.org/packages/makeheaders/)

## Compile with:

```shell
make -j
```

## Test (& compile) with:

```shell
make run -j
```

## Tips

to make a tags file with gmr, do something like:

```shell
gmr -f -tall |& grep -Po '.*\.[ch]' | grep -v main.c | etags -
```

