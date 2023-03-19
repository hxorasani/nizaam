#include "masaadir.h"

#define LABEL(constant) { #constant, constant }
#define LABEL_END { NULL, -1 }

int kul = 0;

masdar *masaadir;

/*static struct label input_prop_labels[] = {
        LABEL(INPUT_PROP_POINTER),
        LABEL(INPUT_PROP_DIRECT),
        LABEL(INPUT_PROP_BUTTONPAD),
        LABEL(INPUT_PROP_SEMI_MT),
        LABEL_END,
};*/
/*char * concat(char str1[], char str2[]) { // TODO move to helper
	char * new_str;
	if ((new_str = malloc(strlen(str1)+strlen(str2)+1)) != NULL){
		new_str[0] = '\0';   // ensures the memory is an empty string
		strcat(new_str,str1);
		strcat(new_str,str2);
	} else {
		printf("malloc failed! %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return new_str;
}*/
/*static const char *get_label(const struct label *labels, int value) {
    while (labels->name && value != labels->value) {
        labels++;
    }
    return labels->name;
}
static int get_input_props(int fd) {
    u_int8_t bits[INPUT_PROP_CNT / 8];
    int i, j;
    int res;
    int count;
    const char *bit_label;

    printf(" ");
    res = ioctl(fd, EVIOCGPROP(sizeof(bits)), bits);
    if (res < 0) {
        return 1;
    }
    count = 0;
    for(i = 0; i < res; i++) {
        for(j = 0; j < 8; j++) {
            if (bits[i] & 1 << j) {
                bit_label = get_label(input_prop_labels, i * 8 + j);
                if(bit_label) {
                    printf("%s ", bit_label);
                } else {
                    printf("%04x ", i * 8 + j);
				}
                count++;
            }
        }
    }
    return 0;
}*/
static int masaadir_info(struct masdar *dev) {
	int fd = open(dev->path, O_RDONLY | O_NOCTTY | O_NONBLOCK);

	if (fd == -1) {
		return EXIT_SUCCESS;
	}
	
	dev->type = UNKNOWN;
	char name[30];
	char location[30];

	if (ioctl(fd, EVIOCGPHYS(30), &location) > -1) {
		dev->location = strdup(location);
	} else dev->location = strdup("");

	struct input_absinfo	absinfox;
	if (ioctl(fd, EVIOCGABS(0), &absinfox) > -1) {
		dev->x = absinfox;
		dev->type = MOUSE;
	}
	struct input_absinfo	absinfoy;
	if (ioctl(fd, EVIOCGABS(1), &absinfoy) > -1) {
		dev->y = absinfoy;
		dev->type = MOUSE;
	}
	struct input_absinfo	absinfom;
	if (ioctl(fd, EVIOCGABS(ABS_MT_SLOT), &absinfom) > -1) {
		dev->maxtouches = absinfom.maximum;
		if (dev->maxtouches) dev->type = TOUCHSCREEN;
	}
//	print_input_props(fd);

	if (ioctl(fd, EVIOCGNAME(30), &name) > -1) {
		dev->name = strdup(name);
		if (strstr(name, "keyboard")) dev->type = KEYBOARD;
		if (strstr(name, "synaptics")) dev->type = TOUCHPAD;
		if (strstr(name, "Synaptics")) dev->type = TOUCHPAD;
	} else dev->name = strdup("");

	close(fd);
	return EXIT_SUCCESS;
}
void masaadir_destroy() {
	for (int i = 0; i < kul; ++i) {
		free(masaadir[i].name);
		free(masaadir[i].location);
		free(masaadir[i].path);
	}
	free(masaadir);
}
masdar * masaadir_get() {
	return masaadir;
}
int masaadir_init() {
	/*
	struct dirent {
	   ino_t          d_ino;       // Inode number
	   off_t          d_off;       // Not an offset;
	   unsigned short d_reclen;    // Length of this record
	   unsigned char  d_type;      // Type of file; not supported
									// by all filesystem types
	   char           d_name[256]; // Null-terminated filename
	};
	*/
	struct dirent **namelist;
	int n;
	
	n = scandir("/dev/input", &namelist, NULL, NULL);
	if (n == -1) {
		perror("scandir");
		return EXIT_FAILURE;
	}
	
	masaadir = (masdar *) calloc(n, sizeof(masdar));
	while (n--) {
		if (strspn(namelist[n]->d_name, "event") == 5) {
			unsigned char *dest = helper_stringify("/dev/input/%s", namelist[n]->d_name);
//			printf("%s: ", namelist[n]->d_name);
			masaadir[kul].path = strdup(dest); // used in the very next func
			masaadir_info(&masaadir[kul]);
			free(dest);
			kul++;
		}

		free(namelist[n]);
	}
	free(namelist);

	// 0 means no devices, so you can fail on no input!
	return kul;
}