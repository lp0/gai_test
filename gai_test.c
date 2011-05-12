/*
 * Copyright ©2010  Simon Arlott
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

#ifdef SOCK_DCCP
	{ "SOCK_DCCP", 0, NULL, 'D' },
#endif
	{ "SOCK_DGRAM", 0, NULL, 'd' },
	{ "SOCK_RAW", 0, NULL, 'R' },
#ifdef SOCK_SEQPACKET
	{ "SOCK_SEQPACKET", 0, NULL, 'S' },
#endif
	{ "SOCK_STREAM", 0, NULL, 's' },

#ifdef IPPROTO_DCCP
	{ "IPPROTO_DCCP", 0, NULL, 'C' },
#endif
#ifdef IPPROTO_SCTP
	{ "IPPROTO_SCTP", 0, NULL, 'P' },
#endif
	{ "IPPROTO_TCP", 0, NULL, 't' },
	{ "IPPROTO_UDP", 0, NULL, 'u' },
#ifdef IPPROTO_UDPLITE
	{ "IPPROTO_UDPLITE", 0, NULL, 'L' },
#endif

#ifdef AI_ADDRCONFIG
	{ "AI_ADDRCONFIG", 0, NULL, 'l' },
#endif
#ifdef AI_ALL
	{ "AI_ALL", 0, NULL, 'a' },
#endif
#ifdef AI_CANONIDN
	{ "AI_CANONIDN", 0, NULL, 'c' },
#endif
	{ "AI_CANONNAME", 0, NULL, 'N' },
#ifdef AI_IDN
	{ "AI_IDN", 0, NULL, 'i' },
#endif
#ifdef AI_IDN_ALLOW_UNASSIGNED
	{ "AI_IDN_ALLOW_UNASSIGNED", 0, NULL, 'U' },
#endif
#ifdef AI_IDN_USE_STD3_ASCII_RULES
	{ "AI_IDN_USE_STD3_ASCII_RULES", 0, NULL, '3' },
#endif
	{ "AI_NUMERICHOST", 0, NULL, 'n' },
#ifdef AI_NUMERICSERV
	{ "AI_NUMERICSERV", 0, NULL, 'v' },
#endif
	{ "AI_PASSIVE", 0, NULL, 'p' },
#ifdef AI_V4MAPPED
	{ "AI_V4MAPPED", 0, NULL, 'm' }
#endif
};

static void help(FILE *out, const char *name) {
			fprintf(out, "Usage: %s [options] [node] [service]\n", name);
			fprintf(out, "\n");
			fprintf(out, "Family:\n");
			fprintf(out, "           AF_UNSPEC (default)\n");
			fprintf(out, "        -4 AF_INET\n");
			fprintf(out, "        -6 AF_INET6\n");
			fprintf(out, "\n");
			fprintf(out, "Socket Type:\n");
#ifdef SOCK_DCCP
			fprintf(out, "        -D SOCK_DCCP\n");
#endif
			fprintf(out, "        -d SOCK_DGRAM\n");
			fprintf(out, "        -R SOCK_RAW\n");
#ifdef SOCK_SEQPACKET
			fprintf(out, "        -S SOCK_SEQPACKET\n");
#endif
			fprintf(out, "        -s SOCK_STREAM\n");
			fprintf(out, "\n");
			fprintf(out, "Protocol:\n");
#ifdef IPPROTO_DCCP
			fprintf(out, "        -C IPPROTO_DCCP\n");
#endif
#ifdef IPPROTO_SCTP
			fprintf(out, "        -P IPPROTO_SCTP\n");
#endif
			fprintf(out, "        -t IPPROTO_TCP\n");
			fprintf(out, "        -u IPPROTO_UDP\n");
#ifdef IPPROTO_UDPLITE
			fprintf(out, "        -L IPPROTO_UDPLITE\n");
#endif
			fprintf(out, "\n");
			fprintf(out, "Flags:\n");
#ifdef AI_ADDRCONFIG
			fprintf(out, "        -l AI_ADDRCONFIG\n");
#endif
#ifdef AI_ALL
			fprintf(out, "        -a AI_ALL\n");
#endif
#ifdef AI_CANONIDN
			fprintf(out, "        -c AI_CANONIDN\n");
#endif
			fprintf(out, "        -N AI_CANONNAME\n");
#ifdef AI_IDN
			fprintf(out, "        -i AI_IDN\n");
#endif
#ifdef AI_IDN_ALLOW_UNASSIGNED
			fprintf(out, "        -U AI_IDN_ALLOW_UNASSIGNED\n");
#endif
#ifdef AI_IDN_USE_STD3_ASCII_RULES
			fprintf(out, "        -3 AI_IDN_USE_STD3_ASCII_RULES\n");
#endif
			fprintf(out, "        -n AI_NUMERICHOST\n");
#ifdef AI_NUMERICSERV
			fprintf(out, "        -v AI_NUMERICSERV\n");
#endif
			fprintf(out, "        -p AI_PASSIVE\n");
#ifdef AI_V4MAPPED
			fprintf(out, "        -m AI_V4MAPPED\n");
#endif
			fprintf(out, "\n");
			fprintf(out, "Options may also be specified in long (--) form by name\n");
			fprintf(out, "For NULL node or service, specify \"\" or \"-\"\n");
			fprintf(out, "Either node or service, but not both, may be NULL\n");
}

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

#ifdef SOCK_DCCP
		case 'D': hints.ai_socktype = SOCK_DCCP; break;
#endif
		case 'd': hints.ai_socktype = SOCK_DGRAM; break;
		case 'R': hints.ai_socktype = SOCK_RAW; break;
#ifdef SOCK_SEQPACKET
		case 'S': hints.ai_socktype = SOCK_SEQPACKET; break;
#endif
		case 's': hints.ai_socktype = SOCK_STREAM; break;

#ifdef IPPROTO_DCCP
		case 'C': hints.ai_protocol = IPPROTO_DCCP; break;
#endif
#ifdef IPPROTO_SCTP
		case 'P': hints.ai_protocol = IPPROTO_SCTP; break;
#endif
		case 't': hints.ai_protocol = IPPROTO_TCP; break;
		case 'u': hints.ai_protocol = IPPROTO_UDP; break;
#ifdef IPPROTO_UDPLITE
		case 'L': hints.ai_protocol = IPPROTO_UDPLITE; break;
#endif

#ifdef AI_ADDRCONFIG
		case 'l': hints.ai_flags |= AI_ADDRCONFIG; break;
#endif
#ifdef AI_ALL
		case 'a': hints.ai_flags |= AI_ALL; break;
#endif
#ifdef AI_CANONIDN
		case 'c': hints.ai_flags |= AI_CANONIDN; break;
#endif
		case 'N': hints.ai_flags |= AI_CANONNAME; break;
#ifdef AI_IDN
		case 'i': hints.ai_flags |= AI_IDN; break;
#endif
#ifdef AI_IDN_ALLOW_UNASSIGNED
		case 'U': hints.ai_flags |= AI_IDN_ALLOW_UNASSIGNED; break;
#endif
#ifdef AI_IDN_USE_STD3_ASCII_RULES
		case '3': hints.ai_flags |= AI_IDN_USE_STD3_ASCII_RULES; break;
#endif
		case 'n': hints.ai_flags |= AI_NUMERICHOST; break;
#ifdef AI_NUMERICSERV
		case 'v': hints.ai_flags |= AI_NUMERICSERV; break;
#endif
		case 'p': hints.ai_flags |= AI_PASSIVE; break;
#ifdef AI_V4MAPPED
		case 'm': hints.ai_flags |= AI_V4MAPPED; break;
#endif

		case 'h':
			help(stdout, argv[0]);
			exit(EXIT_SUCCESS);

		case '?':
			help(stderr, argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (optind < argc) {
		node = argv[optind++];
		if (node[0] == 0 || !strcmp(node, "-"))
			node = NULL;
	}

	if (optind < argc) {
		service = argv[optind++];
		if (service[0] == 0 || !strcmp(service, "-"))
			service = NULL;
	}

	if (node == NULL && service == NULL) {
		help(stderr, argv[0]);
		exit(EXIT_FAILURE);
	}

	ret = getaddrinfo(node, service, &hints, &results);
	if (ret != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(ret));
		exit(ret);
	}

	for (tmp = results; tmp != NULL; tmp = tmp->ai_next) {
		switch (tmp->ai_family) {
		case AF_UNSPEC: printf("AF_UNSPEC "); break;
		case AF_INET:   printf("AF_INET   "); break;
		case AF_INET6:  printf("AF_INET6  "); break;
		default:        printf("AF %-6d ", tmp->ai_family); break;
		}

		switch (tmp->ai_socktype) {
#ifdef SOCK_DCCP
		case SOCK_DCCP:      printf("SOCK_DCCP      "); break;
#endif
		case SOCK_DGRAM:     printf("SOCK_DGRAM     "); break;
		case SOCK_RAW:       printf("SOCK_RAW       "); break;
#ifdef SOCK_SEQPACKET
		case SOCK_SEQPACKET: printf("SOCK_SEQPACKET "); break;
#endif
		case SOCK_STREAM:    printf("SOCK_STREAM    "); break;
		default:             printf("SOCK %-9d ", tmp->ai_socktype); break;
		}

		if (tmp->ai_family == AF_INET || tmp->ai_family == AF_INET6) {
			switch (tmp->ai_protocol) {
#ifdef IPPROTO_DCCP
			case IPPROTO_DCCP:    printf("IPPROTO_DCCP    "); break;
#endif
#ifdef IPPROTO_SCTP
			case IPPROTO_SCTP:    printf("IPPROTO_SCTP    "); break;
#endif
			case IPPROTO_TCP:     printf("IPPROTO_TCP     "); break;
			case IPPROTO_UDP:     printf("IPPROTO_UDP     "); break;
#ifdef IPPROTO_UDPLITE
			case IPPROTO_UDPLITE: printf("IPPROTO_UDPLITE "); break;
#endif
			default:              printf("PROTO %-9d ", tmp->ai_protocol); break;
			}
		} else {
			printf("PROTO %-9d ", tmp->ai_protocol);
		}

		ret = getnameinfo(tmp->ai_addr, tmp->ai_addrlen,
				hbuf, sizeof(hbuf), sbuf, sizeof(sbuf),
				NI_NUMERICHOST | NI_NUMERICSERV);
		if (ret != 0) {
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(ret));
			exit(ret);
		}

		printf("%-39s %-5s", hbuf, sbuf);

		if (tmp->ai_canonname != NULL)
			printf(" (%s)", tmp->ai_canonname);

		printf("\n");
	}

	exit(EXIT_SUCCESS);
}
