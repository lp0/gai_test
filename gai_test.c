#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>

static const struct option long_options[] = {
	{ "AF_UNSPEC", 0, NULL, 0 },
	{ "AF_INET", 0, NULL, '4' },
	{ "AF_INET6", 0, NULL, '6' },

	{ "SOCK_DCCP", 0, NULL, 'D' },
	{ "SOCK_DGRAM", 0, NULL, 'd' },
	{ "SOCK_RAW", 0, NULL, 'R' },
	{ "SOCK_SEQPACKET", 0, NULL, 'S' },
	{ "SOCK_STREAM", 0, NULL, 's' },

	{ "IPPROTO_DCCP", 0, NULL, 'C' },
	{ "IPPROTO_SCTP", 0, NULL, 'P' },
	{ "IPPROTO_TCP", 0, NULL, 't' },
	{ "IPPROTO_UDP", 0, NULL, 'u' },
	{ "IPPROTO_UDPLITE", 0, NULL, 'L' },

	{ "AI_ADDRCONFIG", 0, NULL, 'l' },
	{ "AI_ALL", 0, NULL, 'a' },
	{ "AI_CANONIDN", 0, NULL, 'c' },
	{ "AI_CANONNAME", 0, NULL, 'N' },
	{ "AI_IDN", 0, NULL, 'i' },
	{ "AI_IDN_ALLOW_UNASSIGNED", 0, NULL, 'U' },
	{ "AI_IDN_USE_STD3_ASCII_RULES", 0, NULL, '3' },
	{ "AI_NUMERICHOST", 0, NULL, 'n' },
	{ "AI_NUMERICSERV", 0, NULL, 'v' },
	{ "AI_PASSIVE", 0, NULL, 'p' },
	{ "AI_V4MAPPED", 0, NULL, 'm' }
};

int main(int argc, char *argv[]) {
	struct addrinfo hints;
	struct addrinfo *results, *tmp;
	char *node = NULL, *service = NULL;
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	int ret, opt, option_index = 0;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = 0;
	hints.ai_protocol = 0;
	hints.ai_flags = 0;

	while ((opt = getopt_long(argc, argv, "46DdRSsCPtuLlacNiU3nvpmh",
			long_options, &option_index)) != -1) {
		switch (opt) {
		case 0: break;
		case '4': hints.ai_family = AF_INET; break;
		case '6': hints.ai_family = AF_INET6; break;

		case 'D': hints.ai_socktype = SOCK_DCCP; break;
		case 'd': hints.ai_socktype = SOCK_DGRAM; break;
		case 'R': hints.ai_socktype = SOCK_RAW; break;
		case 'S': hints.ai_socktype = SOCK_SEQPACKET; break;
		case 's': hints.ai_socktype = SOCK_STREAM; break;

		case 'C': hints.ai_protocol = IPPROTO_DCCP; break;
		case 'P': hints.ai_protocol = IPPROTO_SCTP; break;
		case 't': hints.ai_protocol = IPPROTO_TCP; break;
		case 'u': hints.ai_protocol = IPPROTO_UDP; break;
		case 'L': hints.ai_protocol = IPPROTO_UDPLITE; break;

		case 'l': hints.ai_flags |= AI_ADDRCONFIG; break;
		case 'a': hints.ai_flags |= AI_ALL; break;
		case 'c': hints.ai_flags |= AI_CANONIDN; break;
		case 'N': hints.ai_flags |= AI_CANONNAME; break;
		case 'i': hints.ai_flags |= AI_IDN; break;
		case 'U': hints.ai_flags |= AI_IDN_ALLOW_UNASSIGNED; break;
		case '3': hints.ai_flags |= AI_IDN_USE_STD3_ASCII_RULES; break;
		case 'n': hints.ai_flags |= AI_NUMERICHOST; break;
		case 'v': hints.ai_flags |= AI_NUMERICSERV; break;
		case 'p': hints.ai_flags |= AI_PASSIVE; break;
		case 'm': hints.ai_flags |= AI_V4MAPPED; break;

		case 'h': case '?':
			fprintf(stderr, "Usage: %s [options] [node] [service]\n", argv[0]);
			fprintf(stderr, "\n");
			fprintf(stderr, "Family:\n");
			fprintf(stderr, "           AF_UNSPEC (default)\n");
			fprintf(stderr, "        -4 AF_INET\n");
			fprintf(stderr, "        -6 AF_INET6\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "Socket Type:\n");
			fprintf(stderr, "        -D SOCK_DCCP\n");
			fprintf(stderr, "        -d SOCK_DGRAM\n");
			fprintf(stderr, "        -R SOCK_RAW\n");
			fprintf(stderr, "        -S SOCK_SEQPACKET\n");
			fprintf(stderr, "        -s SOCK_STREAM\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "Protocol:\n");
			fprintf(stderr, "        -C IPPROTO_DCCP\n");
			fprintf(stderr, "        -P IPPROTO_SCTP\n");
			fprintf(stderr, "        -t IPPROTO_TCP\n");
			fprintf(stderr, "        -u IPPROTO_UDP\n");
			fprintf(stderr, "        -L IPPROTO_UDPLITE\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "Flags:\n");
			fprintf(stderr, "        -l AI_ADDRCONFIG\n");
			fprintf(stderr, "        -a AI_ALL\n");
			fprintf(stderr, "        -c AI_CANONIDN\n");
			fprintf(stderr, "        -N AI_CANONNAME\n");
			fprintf(stderr, "        -i AI_IDN\n");
			fprintf(stderr, "        -U AI_IDN_ALLOW_UNASSIGNED\n");
			fprintf(stderr, "        -3 AI_IDN_USE_STD3_ASCII_RULES\n");
			fprintf(stderr, "        -n AI_NUMERICHOST\n");
			fprintf(stderr, "        -v AI_NUMERICSERV\n");
			fprintf(stderr, "        -p AI_PASSIVE\n");
			fprintf(stderr, "        -m AI_V4MAPPED\n");
			fprintf(stderr, "\n");
			fprintf(stderr, "Options may also be specified in long (--) form by name\n");
			fprintf(stderr, "Empty node/service is handled as NULL\n");
			exit(EXIT_FAILURE);
		}
	}

	if (optind < argc) {
		node = argv[optind++];
		if (node[0] == 0)
			node = NULL;
	}

	if (optind < argc) {
		service = argv[optind++];
		if (service[0] == 0)
			service = NULL;
	}

	ret = getaddrinfo(node, service, &hints, &results);
	if (ret != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(ret));
		exit(ret);
	}

	for (tmp = results; tmp != NULL; tmp = tmp->ai_next) {
		switch (tmp->ai_family) {
		case AF_UNSPEC: printf("AF_UNSPEC "); break;
		case AF_INET: printf("AF_INET "); break;
		case AF_INET6: printf("AF_INET6 "); break;
		default: printf("AF(%d) ", tmp->ai_family); break;
		}

		switch (tmp->ai_socktype) {
		case SOCK_DCCP: printf("SOCK_DCCP "); break;
		case SOCK_DGRAM: printf("SOCK_DGRAM "); break;
		case SOCK_RAW: printf("SOCK_RAW "); break;
		case SOCK_SEQPACKET: printf("SOCK_SEQPACKET "); break;
		case SOCK_STREAM: printf("SOCK_STREAM "); break;
		default: printf("SOCK(%d) ", tmp->ai_socktype); break;
		}

		switch (tmp->ai_protocol) {
		case IPPROTO_DCCP: printf("IPPROTO_DCCP "); break;
		case IPPROTO_SCTP: printf("IPPROTO_SCTP "); break;
		case IPPROTO_TCP: printf("IPPROTO_TCP "); break;
		case IPPROTO_UDP: printf("IPPROTO_UDP "); break;
		case IPPROTO_UDPLITE: printf("IPPROTO_UDPLITE "); break;
		default: printf("PROTO(%d) ", tmp->ai_protocol); break;
		}

		ret = getnameinfo(tmp->ai_addr, tmp->ai_addrlen,
				hbuf, sizeof(hbuf), sbuf, sizeof(sbuf),
				NI_NUMERICHOST | NI_NUMERICSERV);
		if (ret != 0) {
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(ret));
			exit(ret);
		}

		printf("%s %s", hbuf, sbuf);

		if (tmp->ai_canonname != NULL)
			printf(" (%s)", tmp->ai_canonname);

		printf("\n");
	}

	exit(EXIT_SUCCESS);
}
