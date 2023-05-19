#include <uthash.h>

#include "lookup.h"
#include "services.h"

services_entry_t *services = NULL;
static services_entry_t static_services_entries[1001];

void add_entry(int idx, char *name, struct service *svc) {
	static_services_entries[idx].name = name;
	static_services_entries[idx].svcs = svc;
	HASH_ADD_STR(services, name, &static_services_entries[idx]);
}

void init_services() {
	if (services != NULL) {
		return;
	}
	add_entry(0, "tcpmux", (struct service[]){
		(struct service){1, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(1, "compressnet", (struct service[]){
		(struct service){2, IPPROTO_TCP, SOCK_STREAM},
		(struct service){2, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){3, IPPROTO_TCP, SOCK_STREAM},
		(struct service){3, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(2, "rje", (struct service[]){
		(struct service){5, IPPROTO_TCP, SOCK_STREAM},
		(struct service){5, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(3, "echo", (struct service[]){
		(struct service){7, IPPROTO_TCP, SOCK_STREAM},
		(struct service){7, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(4, "discard", (struct service[]){
		(struct service){9, IPPROTO_TCP, SOCK_STREAM},
		(struct service){9, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(5, "systat", (struct service[]){
		(struct service){11, IPPROTO_TCP, SOCK_STREAM},
		(struct service){11, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(6, "daytime", (struct service[]){
		(struct service){13, IPPROTO_TCP, SOCK_STREAM},
		(struct service){13, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(7, "qotd", (struct service[]){
		(struct service){17, IPPROTO_TCP, SOCK_STREAM},
		(struct service){17, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(8, "chargen", (struct service[]){
		(struct service){19, IPPROTO_TCP, SOCK_STREAM},
		(struct service){19, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(9, "ftp-data", (struct service[]){
		(struct service){20, IPPROTO_TCP, SOCK_STREAM},
		(struct service){20, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(10, "ftp", (struct service[]){
		(struct service){21, IPPROTO_TCP, SOCK_STREAM},
		(struct service){21, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(11, "ssh", (struct service[]){
		(struct service){22, IPPROTO_TCP, SOCK_STREAM},
		(struct service){22, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(12, "telnet", (struct service[]){
		(struct service){23, IPPROTO_TCP, SOCK_STREAM},
		(struct service){23, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(13, "smtp", (struct service[]){
		(struct service){25, IPPROTO_TCP, SOCK_STREAM},
		(struct service){25, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(14, "nsw-fe", (struct service[]){
		(struct service){27, IPPROTO_TCP, SOCK_STREAM},
		(struct service){27, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(15, "msg-icp", (struct service[]){
		(struct service){29, IPPROTO_TCP, SOCK_STREAM},
		(struct service){29, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(16, "msg-auth", (struct service[]){
		(struct service){31, IPPROTO_TCP, SOCK_STREAM},
		(struct service){31, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(17, "dsp", (struct service[]){
		(struct service){33, IPPROTO_TCP, SOCK_STREAM},
		(struct service){33, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(18, "time", (struct service[]){
		(struct service){37, IPPROTO_TCP, SOCK_STREAM},
		(struct service){37, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(19, "rap", (struct service[]){
		(struct service){38, IPPROTO_TCP, SOCK_STREAM},
		(struct service){38, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){256, IPPROTO_TCP, SOCK_STREAM},
		(struct service){256, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(20, "rlp", (struct service[]){
		(struct service){39, IPPROTO_TCP, SOCK_STREAM},
		(struct service){39, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(21, "graphics", (struct service[]){
		(struct service){41, IPPROTO_TCP, SOCK_STREAM},
		(struct service){41, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(22, "name", (struct service[]){
		(struct service){42, IPPROTO_TCP, SOCK_STREAM},
		(struct service){42, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(23, "nameserver", (struct service[]){
		(struct service){42, IPPROTO_TCP, SOCK_STREAM},
		(struct service){42, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(24, "nicname", (struct service[]){
		(struct service){43, IPPROTO_TCP, SOCK_STREAM},
		(struct service){43, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(25, "mpm-flags", (struct service[]){
		(struct service){44, IPPROTO_TCP, SOCK_STREAM},
		(struct service){44, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(26, "mpm", (struct service[]){
		(struct service){45, IPPROTO_TCP, SOCK_STREAM},
		(struct service){45, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(27, "mpm-snd", (struct service[]){
		(struct service){46, IPPROTO_TCP, SOCK_STREAM},
		(struct service){46, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(28, "auditd", (struct service[]){
		(struct service){48, IPPROTO_TCP, SOCK_STREAM},
		(struct service){48, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(29, "tacacs", (struct service[]){
		(struct service){49, IPPROTO_TCP, SOCK_STREAM},
		(struct service){49, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(30, "re-mail-ck", (struct service[]){
		(struct service){50, IPPROTO_TCP, SOCK_STREAM},
		(struct service){50, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(31, "xns-time", (struct service[]){
		(struct service){52, IPPROTO_TCP, SOCK_STREAM},
		(struct service){52, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(32, "domain", (struct service[]){
		(struct service){53, IPPROTO_TCP, SOCK_STREAM},
		(struct service){53, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(33, "xns-ch", (struct service[]){
		(struct service){54, IPPROTO_TCP, SOCK_STREAM},
		(struct service){54, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(34, "isi-gl", (struct service[]){
		(struct service){55, IPPROTO_TCP, SOCK_STREAM},
		(struct service){55, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(35, "xns-auth", (struct service[]){
		(struct service){56, IPPROTO_TCP, SOCK_STREAM},
		(struct service){56, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(36, "xns-mail", (struct service[]){
		(struct service){58, IPPROTO_TCP, SOCK_STREAM},
		(struct service){58, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(37, "acas", (struct service[]){
		(struct service){62, IPPROTO_TCP, SOCK_STREAM},
		(struct service){62, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(38, "whoispp", (struct service[]){
		(struct service){63, IPPROTO_TCP, SOCK_STREAM},
		(struct service){63, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(39, "covia", (struct service[]){
		(struct service){64, IPPROTO_TCP, SOCK_STREAM},
		(struct service){64, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(40, "tacacs-ds", (struct service[]){
		(struct service){65, IPPROTO_TCP, SOCK_STREAM},
		(struct service){65, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(41, "sql-net", (struct service[]){
		(struct service){66, IPPROTO_TCP, SOCK_STREAM},
		(struct service){66, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){150, IPPROTO_TCP, SOCK_STREAM},
		(struct service){150, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(42, "bootps", (struct service[]){
		(struct service){67, IPPROTO_TCP, SOCK_STREAM},
		(struct service){67, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(43, "bootpc", (struct service[]){
		(struct service){68, IPPROTO_TCP, SOCK_STREAM},
		(struct service){68, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(44, "tftp", (struct service[]){
		(struct service){69, IPPROTO_TCP, SOCK_STREAM},
		(struct service){69, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(45, "gopher", (struct service[]){
		(struct service){70, IPPROTO_TCP, SOCK_STREAM},
		(struct service){70, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(46, "netrjs-1", (struct service[]){
		(struct service){71, IPPROTO_TCP, SOCK_STREAM},
		(struct service){71, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(47, "netrjs-2", (struct service[]){
		(struct service){72, IPPROTO_TCP, SOCK_STREAM},
		(struct service){72, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(48, "netrjs-3", (struct service[]){
		(struct service){73, IPPROTO_TCP, SOCK_STREAM},
		(struct service){73, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(49, "netrjs-4", (struct service[]){
		(struct service){74, IPPROTO_TCP, SOCK_STREAM},
		(struct service){74, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(50, "deos", (struct service[]){
		(struct service){76, IPPROTO_TCP, SOCK_STREAM},
		(struct service){76, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(51, "vettcp", (struct service[]){
		(struct service){78, IPPROTO_TCP, SOCK_STREAM},
		(struct service){78, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(52, "finger", (struct service[]){
		(struct service){79, IPPROTO_TCP, SOCK_STREAM},
		(struct service){79, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(53, "http", (struct service[]){
		(struct service){80, IPPROTO_TCP, SOCK_STREAM},
		(struct service){80, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(54, "www", (struct service[]){
		(struct service){80, IPPROTO_TCP, SOCK_STREAM},
		(struct service){80, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(55, "www-http", (struct service[]){
		(struct service){80, IPPROTO_TCP, SOCK_STREAM},
		(struct service){80, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(56, "xfer", (struct service[]){
		(struct service){82, IPPROTO_TCP, SOCK_STREAM},
		(struct service){82, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(57, "mit-ml-dev", (struct service[]){
		(struct service){83, IPPROTO_TCP, SOCK_STREAM},
		(struct service){83, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){85, IPPROTO_TCP, SOCK_STREAM},
		(struct service){85, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(58, "ctf", (struct service[]){
		(struct service){84, IPPROTO_TCP, SOCK_STREAM},
		(struct service){84, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(59, "mfcobol", (struct service[]){
		(struct service){86, IPPROTO_TCP, SOCK_STREAM},
		(struct service){86, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(60, "kerberos", (struct service[]){
		(struct service){88, IPPROTO_TCP, SOCK_STREAM},
		(struct service){88, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(61, "su-mit-tg", (struct service[]){
		(struct service){89, IPPROTO_TCP, SOCK_STREAM},
		(struct service){89, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(62, "dnsix", (struct service[]){
		(struct service){90, IPPROTO_TCP, SOCK_STREAM},
		(struct service){90, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(63, "mit-dov", (struct service[]){
		(struct service){91, IPPROTO_TCP, SOCK_STREAM},
		(struct service){91, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(64, "npp", (struct service[]){
		(struct service){92, IPPROTO_TCP, SOCK_STREAM},
		(struct service){92, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){4045, IPPROTO_TCP, SOCK_STREAM},
		(struct service){4045, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(65, "dcp", (struct service[]){
		(struct service){93, IPPROTO_TCP, SOCK_STREAM},
		(struct service){93, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(66, "objcall", (struct service[]){
		(struct service){94, IPPROTO_TCP, SOCK_STREAM},
		(struct service){94, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(67, "supdup", (struct service[]){
		(struct service){95, IPPROTO_TCP, SOCK_STREAM},
		(struct service){95, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(68, "dixie", (struct service[]){
		(struct service){96, IPPROTO_TCP, SOCK_STREAM},
		(struct service){96, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(69, "swift-rvf", (struct service[]){
		(struct service){97, IPPROTO_TCP, SOCK_STREAM},
		(struct service){97, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(70, "tacnews", (struct service[]){
		(struct service){98, IPPROTO_TCP, SOCK_STREAM},
		(struct service){98, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(71, "metagram", (struct service[]){
		(struct service){99, IPPROTO_TCP, SOCK_STREAM},
		(struct service){99, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(72, "hostname", (struct service[]){
		(struct service){101, IPPROTO_TCP, SOCK_STREAM},
		(struct service){101, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(73, "iso-tsap", (struct service[]){
		(struct service){102, IPPROTO_TCP, SOCK_STREAM},
		(struct service){102, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(74, "gppitnp", (struct service[]){
		(struct service){103, IPPROTO_TCP, SOCK_STREAM},
		(struct service){103, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(75, "acr-nema", (struct service[]){
		(struct service){104, IPPROTO_TCP, SOCK_STREAM},
		(struct service){104, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(76, "cso", (struct service[]){
		(struct service){105, IPPROTO_TCP, SOCK_STREAM},
		(struct service){105, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(77, "csnet-ns", (struct service[]){
		(struct service){105, IPPROTO_TCP, SOCK_STREAM},
		(struct service){105, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(78, "3com-tsmux", (struct service[]){
		(struct service){106, IPPROTO_TCP, SOCK_STREAM},
		(struct service){106, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(79, "rtelnet", (struct service[]){
		(struct service){107, IPPROTO_TCP, SOCK_STREAM},
		(struct service){107, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(80, "snagas", (struct service[]){
		(struct service){108, IPPROTO_TCP, SOCK_STREAM},
		(struct service){108, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(81, "pop2", (struct service[]){
		(struct service){109, IPPROTO_TCP, SOCK_STREAM},
		(struct service){109, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(82, "pop3", (struct service[]){
		(struct service){110, IPPROTO_TCP, SOCK_STREAM},
		(struct service){110, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(83, "sunrpc", (struct service[]){
		(struct service){111, IPPROTO_TCP, SOCK_STREAM},
		(struct service){111, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(84, "mcidas", (struct service[]){
		(struct service){112, IPPROTO_TCP, SOCK_STREAM},
		(struct service){112, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(85, "ident", (struct service[]){
		(struct service){113, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(86, "auth", (struct service[]){
		(struct service){113, IPPROTO_TCP, SOCK_STREAM},
		(struct service){113, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(87, "sftp", (struct service[]){
		(struct service){115, IPPROTO_TCP, SOCK_STREAM},
		(struct service){115, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(88, "ansanotify", (struct service[]){
		(struct service){116, IPPROTO_TCP, SOCK_STREAM},
		(struct service){116, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(89, "uucp-path", (struct service[]){
		(struct service){117, IPPROTO_TCP, SOCK_STREAM},
		(struct service){117, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(90, "sqlserv", (struct service[]){
		(struct service){118, IPPROTO_TCP, SOCK_STREAM},
		(struct service){118, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(91, "nntp", (struct service[]){
		(struct service){119, IPPROTO_TCP, SOCK_STREAM},
		(struct service){119, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(92, "cfdptkt", (struct service[]){
		(struct service){120, IPPROTO_TCP, SOCK_STREAM},
		(struct service){120, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(93, "erpc", (struct service[]){
		(struct service){121, IPPROTO_TCP, SOCK_STREAM},
		(struct service){121, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(94, "smakynet", (struct service[]){
		(struct service){122, IPPROTO_TCP, SOCK_STREAM},
		(struct service){122, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(95, "ntp", (struct service[]){
		(struct service){123, IPPROTO_TCP, SOCK_STREAM},
		(struct service){123, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(96, "ansatrader", (struct service[]){
		(struct service){124, IPPROTO_TCP, SOCK_STREAM},
		(struct service){124, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(97, "locus-map", (struct service[]){
		(struct service){125, IPPROTO_TCP, SOCK_STREAM},
		(struct service){125, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(98, "nxedit", (struct service[]){
		(struct service){126, IPPROTO_TCP, SOCK_STREAM},
		(struct service){126, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(99, "locus-con", (struct service[]){
		(struct service){127, IPPROTO_TCP, SOCK_STREAM},
		(struct service){127, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(100, "gss-xlicen", (struct service[]){
		(struct service){128, IPPROTO_TCP, SOCK_STREAM},
		(struct service){128, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(101, "pwdgen", (struct service[]){
		(struct service){129, IPPROTO_TCP, SOCK_STREAM},
		(struct service){129, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(102, "cisco-fna", (struct service[]){
		(struct service){130, IPPROTO_TCP, SOCK_STREAM},
		(struct service){130, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(103, "cisco-tna", (struct service[]){
		(struct service){131, IPPROTO_TCP, SOCK_STREAM},
		(struct service){131, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(104, "cisco-sys", (struct service[]){
		(struct service){132, IPPROTO_TCP, SOCK_STREAM},
		(struct service){132, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(105, "statsrv", (struct service[]){
		(struct service){133, IPPROTO_TCP, SOCK_STREAM},
		(struct service){133, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(106, "ingres-net", (struct service[]){
		(struct service){134, IPPROTO_TCP, SOCK_STREAM},
		(struct service){134, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(107, "epmap", (struct service[]){
		(struct service){135, IPPROTO_TCP, SOCK_STREAM},
		(struct service){135, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(108, "profile", (struct service[]){
		(struct service){136, IPPROTO_TCP, SOCK_STREAM},
		(struct service){136, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(109, "netbios-ns", (struct service[]){
		(struct service){137, IPPROTO_TCP, SOCK_STREAM},
		(struct service){137, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(110, "netbios-dgm", (struct service[]){
		(struct service){138, IPPROTO_TCP, SOCK_STREAM},
		(struct service){138, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(111, "netbios-ssn", (struct service[]){
		(struct service){139, IPPROTO_TCP, SOCK_STREAM},
		(struct service){139, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(112, "emfis-data", (struct service[]){
		(struct service){140, IPPROTO_TCP, SOCK_STREAM},
		(struct service){140, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(113, "emfis-cntl", (struct service[]){
		(struct service){141, IPPROTO_TCP, SOCK_STREAM},
		(struct service){141, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(114, "bl-idm", (struct service[]){
		(struct service){142, IPPROTO_TCP, SOCK_STREAM},
		(struct service){142, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(115, "imap", (struct service[]){
		(struct service){143, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(116, "uma", (struct service[]){
		(struct service){144, IPPROTO_TCP, SOCK_STREAM},
		(struct service){144, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){1797, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1797, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(117, "uaac", (struct service[]){
		(struct service){145, IPPROTO_TCP, SOCK_STREAM},
		(struct service){145, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(118, "iso-tp0", (struct service[]){
		(struct service){146, IPPROTO_TCP, SOCK_STREAM},
		(struct service){146, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(119, "iso-ip", (struct service[]){
		(struct service){147, IPPROTO_TCP, SOCK_STREAM},
		(struct service){147, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(120, "jargon", (struct service[]){
		(struct service){148, IPPROTO_TCP, SOCK_STREAM},
		(struct service){148, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(121, "aed-512", (struct service[]){
		(struct service){149, IPPROTO_TCP, SOCK_STREAM},
		(struct service){149, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(122, "hems", (struct service[]){
		(struct service){151, IPPROTO_TCP, SOCK_STREAM},
		(struct service){151, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(123, "bftp", (struct service[]){
		(struct service){152, IPPROTO_TCP, SOCK_STREAM},
		(struct service){152, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(124, "sgmp", (struct service[]){
		(struct service){153, IPPROTO_TCP, SOCK_STREAM},
		(struct service){153, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(125, "netsc-prod", (struct service[]){
		(struct service){154, IPPROTO_TCP, SOCK_STREAM},
		(struct service){154, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(126, "netsc-dev", (struct service[]){
		(struct service){155, IPPROTO_TCP, SOCK_STREAM},
		(struct service){155, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(127, "sqlsrv", (struct service[]){
		(struct service){156, IPPROTO_TCP, SOCK_STREAM},
		(struct service){156, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(128, "knet-cmp", (struct service[]){
		(struct service){157, IPPROTO_TCP, SOCK_STREAM},
		(struct service){157, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(129, "pcmail-srv", (struct service[]){
		(struct service){158, IPPROTO_TCP, SOCK_STREAM},
		(struct service){158, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(130, "nss-routing", (struct service[]){
		(struct service){159, IPPROTO_TCP, SOCK_STREAM},
		(struct service){159, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(131, "sgmp-traps", (struct service[]){
		(struct service){160, IPPROTO_TCP, SOCK_STREAM},
		(struct service){160, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(132, "snmp", (struct service[]){
		(struct service){161, IPPROTO_TCP, SOCK_STREAM},
		(struct service){161, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(133, "snmptrap", (struct service[]){
		(struct service){162, IPPROTO_TCP, SOCK_STREAM},
		(struct service){162, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(134, "cmip-man", (struct service[]){
		(struct service){163, IPPROTO_TCP, SOCK_STREAM},
		(struct service){163, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(135, "cmip-agent", (struct service[]){
		(struct service){164, IPPROTO_TCP, SOCK_STREAM},
		(struct service){164, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(136, "xns-courier", (struct service[]){
		(struct service){165, IPPROTO_TCP, SOCK_STREAM},
		(struct service){165, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(137, "s-net", (struct service[]){
		(struct service){166, IPPROTO_TCP, SOCK_STREAM},
		(struct service){166, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(138, "namp", (struct service[]){
		(struct service){167, IPPROTO_TCP, SOCK_STREAM},
		(struct service){167, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(139, "rsvd", (struct service[]){
		(struct service){168, IPPROTO_TCP, SOCK_STREAM},
		(struct service){168, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(140, "send", (struct service[]){
		(struct service){169, IPPROTO_TCP, SOCK_STREAM},
		(struct service){169, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(141, "print-srv", (struct service[]){
		(struct service){170, IPPROTO_TCP, SOCK_STREAM},
		(struct service){170, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(142, "multiplex", (struct service[]){
		(struct service){171, IPPROTO_TCP, SOCK_STREAM},
		(struct service){171, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(143, "cl-1", (struct service[]){
		(struct service){172, IPPROTO_TCP, SOCK_STREAM},
		(struct service){172, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(144, "xyplex-mux", (struct service[]){
		(struct service){173, IPPROTO_TCP, SOCK_STREAM},
		(struct service){173, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(145, "mailq", (struct service[]){
		(struct service){174, IPPROTO_TCP, SOCK_STREAM},
		(struct service){174, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(146, "vmnet", (struct service[]){
		(struct service){175, IPPROTO_TCP, SOCK_STREAM},
		(struct service){175, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(147, "genrad-mux", (struct service[]){
		(struct service){176, IPPROTO_TCP, SOCK_STREAM},
		(struct service){176, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(148, "xdmcp", (struct service[]){
		(struct service){177, IPPROTO_TCP, SOCK_STREAM},
		(struct service){177, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(149, "nextstep", (struct service[]){
		(struct service){178, IPPROTO_TCP, SOCK_STREAM},
		(struct service){178, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(150, "bgp", (struct service[]){
		(struct service){179, IPPROTO_TCP, SOCK_STREAM},
		(struct service){179, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(151, "ris", (struct service[]){
		(struct service){180, IPPROTO_TCP, SOCK_STREAM},
		(struct service){180, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(152, "unify", (struct service[]){
		(struct service){181, IPPROTO_TCP, SOCK_STREAM},
		(struct service){181, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(153, "audit", (struct service[]){
		(struct service){182, IPPROTO_TCP, SOCK_STREAM},
		(struct service){182, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(154, "ocbinder", (struct service[]){
		(struct service){183, IPPROTO_TCP, SOCK_STREAM},
		(struct service){183, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(155, "ocserver", (struct service[]){
		(struct service){184, IPPROTO_TCP, SOCK_STREAM},
		(struct service){184, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(156, "remote-kis", (struct service[]){
		(struct service){185, IPPROTO_TCP, SOCK_STREAM},
		(struct service){185, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(157, "kis", (struct service[]){
		(struct service){186, IPPROTO_TCP, SOCK_STREAM},
		(struct service){186, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(158, "aci", (struct service[]){
		(struct service){187, IPPROTO_TCP, SOCK_STREAM},
		(struct service){187, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(159, "mumps", (struct service[]){
		(struct service){188, IPPROTO_TCP, SOCK_STREAM},
		(struct service){188, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(160, "qft", (struct service[]){
		(struct service){189, IPPROTO_TCP, SOCK_STREAM},
		(struct service){189, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(161, "gacp", (struct service[]){
		(struct service){190, IPPROTO_TCP, SOCK_STREAM},
		(struct service){190, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(162, "prospero", (struct service[]){
		(struct service){191, IPPROTO_TCP, SOCK_STREAM},
		(struct service){191, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(163, "osu-nms", (struct service[]){
		(struct service){192, IPPROTO_TCP, SOCK_STREAM},
		(struct service){192, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(164, "srmp", (struct service[]){
		(struct service){193, IPPROTO_TCP, SOCK_STREAM},
		(struct service){193, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(165, "irc", (struct service[]){
		(struct service){194, IPPROTO_TCP, SOCK_STREAM},
		(struct service){194, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(166, "dn6-nlm-aud", (struct service[]){
		(struct service){195, IPPROTO_TCP, SOCK_STREAM},
		(struct service){195, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(167, "dn6-smm-red", (struct service[]){
		(struct service){196, IPPROTO_TCP, SOCK_STREAM},
		(struct service){196, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(168, "dls", (struct service[]){
		(struct service){197, IPPROTO_TCP, SOCK_STREAM},
		(struct service){197, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){2047, IPPROTO_TCP, SOCK_STREAM},
		(struct service){2047, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(169, "dls-mon", (struct service[]){
		(struct service){198, IPPROTO_TCP, SOCK_STREAM},
		(struct service){198, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(170, "smux", (struct service[]){
		(struct service){199, IPPROTO_TCP, SOCK_STREAM},
		(struct service){199, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(171, "src", (struct service[]){
		(struct service){200, IPPROTO_TCP, SOCK_STREAM},
		(struct service){200, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(172, "at-rtmp", (struct service[]){
		(struct service){201, IPPROTO_TCP, SOCK_STREAM},
		(struct service){201, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(173, "at-nbp", (struct service[]){
		(struct service){202, IPPROTO_TCP, SOCK_STREAM},
		(struct service){202, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(174, "at-3", (struct service[]){
		(struct service){203, IPPROTO_TCP, SOCK_STREAM},
		(struct service){203, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(175, "at-echo", (struct service[]){
		(struct service){204, IPPROTO_TCP, SOCK_STREAM},
		(struct service){204, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(176, "at-5", (struct service[]){
		(struct service){205, IPPROTO_TCP, SOCK_STREAM},
		(struct service){205, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(177, "at-zis", (struct service[]){
		(struct service){206, IPPROTO_TCP, SOCK_STREAM},
		(struct service){206, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(178, "at-7", (struct service[]){
		(struct service){207, IPPROTO_TCP, SOCK_STREAM},
		(struct service){207, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(179, "at-8", (struct service[]){
		(struct service){208, IPPROTO_TCP, SOCK_STREAM},
		(struct service){208, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(180, "qmtp", (struct service[]){
		(struct service){209, IPPROTO_TCP, SOCK_STREAM},
		(struct service){209, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(181, "z39-50", (struct service[]){
		(struct service){210, IPPROTO_TCP, SOCK_STREAM},
		(struct service){210, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(182, "914c-g", (struct service[]){
		(struct service){211, IPPROTO_TCP, SOCK_STREAM},
		(struct service){211, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(183, "anet", (struct service[]){
		(struct service){212, IPPROTO_TCP, SOCK_STREAM},
		(struct service){212, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(184, "ipx", (struct service[]){
		(struct service){213, IPPROTO_TCP, SOCK_STREAM},
		(struct service){213, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(185, "vmpwscs", (struct service[]){
		(struct service){214, IPPROTO_TCP, SOCK_STREAM},
		(struct service){214, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(186, "softpc", (struct service[]){
		(struct service){215, IPPROTO_TCP, SOCK_STREAM},
		(struct service){215, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(187, "CAIlic", (struct service[]){
		(struct service){216, IPPROTO_TCP, SOCK_STREAM},
		(struct service){216, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(188, "dbase", (struct service[]){
		(struct service){217, IPPROTO_TCP, SOCK_STREAM},
		(struct service){217, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(189, "mpp", (struct service[]){
		(struct service){218, IPPROTO_TCP, SOCK_STREAM},
		(struct service){218, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(190, "uarps", (struct service[]){
		(struct service){219, IPPROTO_TCP, SOCK_STREAM},
		(struct service){219, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(191, "imap3", (struct service[]){
		(struct service){220, IPPROTO_TCP, SOCK_STREAM},
		(struct service){220, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(192, "fln-spx", (struct service[]){
		(struct service){221, IPPROTO_TCP, SOCK_STREAM},
		(struct service){221, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(193, "rsh-spx", (struct service[]){
		(struct service){222, IPPROTO_TCP, SOCK_STREAM},
		(struct service){222, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(194, "cdc", (struct service[]){
		(struct service){223, IPPROTO_TCP, SOCK_STREAM},
		(struct service){223, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(195, "masqdialer", (struct service[]){
		(struct service){224, IPPROTO_TCP, SOCK_STREAM},
		(struct service){224, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(196, "direct", (struct service[]){
		(struct service){242, IPPROTO_TCP, SOCK_STREAM},
		(struct service){242, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(197, "sur-meas", (struct service[]){
		(struct service){243, IPPROTO_TCP, SOCK_STREAM},
		(struct service){243, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(198, "inbusiness", (struct service[]){
		(struct service){244, IPPROTO_TCP, SOCK_STREAM},
		(struct service){244, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(199, "link", (struct service[]){
		(struct service){245, IPPROTO_TCP, SOCK_STREAM},
		(struct service){245, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(200, "dsp3270", (struct service[]){
		(struct service){246, IPPROTO_TCP, SOCK_STREAM},
		(struct service){246, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(201, "subntbcst-tftp", (struct service[]){
		(struct service){247, IPPROTO_TCP, SOCK_STREAM},
		(struct service){247, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(202, "bhfhs", (struct service[]){
		(struct service){248, IPPROTO_TCP, SOCK_STREAM},
		(struct service){248, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(203, "set", (struct service[]){
		(struct service){257, IPPROTO_TCP, SOCK_STREAM},
		(struct service){257, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(204, "esro-gen", (struct service[]){
		(struct service){259, IPPROTO_TCP, SOCK_STREAM},
		(struct service){259, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(205, "openport", (struct service[]){
		(struct service){260, IPPROTO_TCP, SOCK_STREAM},
		(struct service){260, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(206, "nsiiops", (struct service[]){
		(struct service){261, IPPROTO_TCP, SOCK_STREAM},
		(struct service){261, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(207, "arcisdms", (struct service[]){
		(struct service){262, IPPROTO_TCP, SOCK_STREAM},
		(struct service){262, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(208, "hdap", (struct service[]){
		(struct service){263, IPPROTO_TCP, SOCK_STREAM},
		(struct service){263, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(209, "bgmp", (struct service[]){
		(struct service){264, IPPROTO_TCP, SOCK_STREAM},
		(struct service){264, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(210, "x-bone-ctl", (struct service[]){
		(struct service){265, IPPROTO_TCP, SOCK_STREAM},
		(struct service){265, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(211, "sst", (struct service[]){
		(struct service){266, IPPROTO_TCP, SOCK_STREAM},
		(struct service){266, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(212, "td-service", (struct service[]){
		(struct service){267, IPPROTO_TCP, SOCK_STREAM},
		(struct service){267, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(213, "td-replica", (struct service[]){
		(struct service){268, IPPROTO_TCP, SOCK_STREAM},
		(struct service){268, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(214, "manet", (struct service[]){
		(struct service){269, IPPROTO_TCP, SOCK_STREAM},
		(struct service){269, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(215, "gist", (struct service[]){
		(struct service){270, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(216, "pt-tls", (struct service[]){
		(struct service){271, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(217, "http-mgmt", (struct service[]){
		(struct service){280, IPPROTO_TCP, SOCK_STREAM},
		(struct service){280, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(218, "personal-link", (struct service[]){
		(struct service){281, IPPROTO_TCP, SOCK_STREAM},
		(struct service){281, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(219, "cableport-ax", (struct service[]){
		(struct service){282, IPPROTO_TCP, SOCK_STREAM},
		(struct service){282, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(220, "rescap", (struct service[]){
		(struct service){283, IPPROTO_TCP, SOCK_STREAM},
		(struct service){283, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(221, "corerjd", (struct service[]){
		(struct service){284, IPPROTO_TCP, SOCK_STREAM},
		(struct service){284, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(222, "fxp", (struct service[]){
		(struct service){286, IPPROTO_TCP, SOCK_STREAM},
		(struct service){286, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){2849, IPPROTO_TCP, SOCK_STREAM},
		(struct service){2849, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(223, "k-block", (struct service[]){
		(struct service){287, IPPROTO_TCP, SOCK_STREAM},
		(struct service){287, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(224, "novastorbakcup", (struct service[]){
		(struct service){308, IPPROTO_TCP, SOCK_STREAM},
		(struct service){308, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(225, "entrusttime", (struct service[]){
		(struct service){309, IPPROTO_TCP, SOCK_STREAM},
		(struct service){309, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(226, "bhmds", (struct service[]){
		(struct service){310, IPPROTO_TCP, SOCK_STREAM},
		(struct service){310, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(227, "asip-webadmin", (struct service[]){
		(struct service){311, IPPROTO_TCP, SOCK_STREAM},
		(struct service){311, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(228, "vslmp", (struct service[]){
		(struct service){312, IPPROTO_TCP, SOCK_STREAM},
		(struct service){312, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(229, "magenta-logic", (struct service[]){
		(struct service){313, IPPROTO_TCP, SOCK_STREAM},
		(struct service){313, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(230, "opalis-robot", (struct service[]){
		(struct service){314, IPPROTO_TCP, SOCK_STREAM},
		(struct service){314, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(231, "dpsi", (struct service[]){
		(struct service){315, IPPROTO_TCP, SOCK_STREAM},
		(struct service){315, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(232, "decauth", (struct service[]){
		(struct service){316, IPPROTO_TCP, SOCK_STREAM},
		(struct service){316, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(233, "zannet", (struct service[]){
		(struct service){317, IPPROTO_TCP, SOCK_STREAM},
		(struct service){317, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(234, "pkix-timestamp", (struct service[]){
		(struct service){318, IPPROTO_TCP, SOCK_STREAM},
		(struct service){318, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(235, "ptp-event", (struct service[]){
		(struct service){319, IPPROTO_TCP, SOCK_STREAM},
		(struct service){319, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(236, "ptp-general", (struct service[]){
		(struct service){320, IPPROTO_TCP, SOCK_STREAM},
		(struct service){320, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(237, "pip", (struct service[]){
		(struct service){321, IPPROTO_TCP, SOCK_STREAM},
		(struct service){321, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){1321, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1321, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(238, "rtsps", (struct service[]){
		(struct service){322, IPPROTO_TCP, SOCK_STREAM},
		(struct service){322, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(239, "rpki-rtr", (struct service[]){
		(struct service){323, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(240, "rpki-rtr-tls", (struct service[]){
		(struct service){324, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(241, "texar", (struct service[]){
		(struct service){333, IPPROTO_TCP, SOCK_STREAM},
		(struct service){333, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(242, "pdap", (struct service[]){
		(struct service){344, IPPROTO_TCP, SOCK_STREAM},
		(struct service){344, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(243, "pawserv", (struct service[]){
		(struct service){345, IPPROTO_TCP, SOCK_STREAM},
		(struct service){345, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(244, "zserv", (struct service[]){
		(struct service){346, IPPROTO_TCP, SOCK_STREAM},
		(struct service){346, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(245, "fatserv", (struct service[]){
		(struct service){347, IPPROTO_TCP, SOCK_STREAM},
		(struct service){347, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(246, "csi-sgwp", (struct service[]){
		(struct service){348, IPPROTO_TCP, SOCK_STREAM},
		(struct service){348, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(247, "mftp", (struct service[]){
		(struct service){349, IPPROTO_TCP, SOCK_STREAM},
		(struct service){349, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){5402, IPPROTO_TCP, SOCK_STREAM},
		(struct service){5402, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(248, "matip-type-a", (struct service[]){
		(struct service){350, IPPROTO_TCP, SOCK_STREAM},
		(struct service){350, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(249, "matip-type-b", (struct service[]){
		(struct service){351, IPPROTO_TCP, SOCK_STREAM},
		(struct service){351, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(250, "bhoetty", (struct service[]){
		(struct service){351, IPPROTO_TCP, SOCK_STREAM},
		(struct service){351, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(251, "dtag-ste-sb", (struct service[]){
		(struct service){352, IPPROTO_TCP, SOCK_STREAM},
		(struct service){352, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(252, "bhoedap4", (struct service[]){
		(struct service){352, IPPROTO_TCP, SOCK_STREAM},
		(struct service){352, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(253, "ndsauth", (struct service[]){
		(struct service){353, IPPROTO_TCP, SOCK_STREAM},
		(struct service){353, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(254, "bh611", (struct service[]){
		(struct service){354, IPPROTO_TCP, SOCK_STREAM},
		(struct service){354, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(255, "datex-asn", (struct service[]){
		(struct service){355, IPPROTO_TCP, SOCK_STREAM},
		(struct service){355, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(256, "cloanto-net-1", (struct service[]){
		(struct service){356, IPPROTO_TCP, SOCK_STREAM},
		(struct service){356, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(257, "bhevent", (struct service[]){
		(struct service){357, IPPROTO_TCP, SOCK_STREAM},
		(struct service){357, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(258, "shrinkwrap", (struct service[]){
		(struct service){358, IPPROTO_TCP, SOCK_STREAM},
		(struct service){358, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(259, "nsrmp", (struct service[]){
		(struct service){359, IPPROTO_TCP, SOCK_STREAM},
		(struct service){359, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(260, "scoi2odialog", (struct service[]){
		(struct service){360, IPPROTO_TCP, SOCK_STREAM},
		(struct service){360, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(261, "semantix", (struct service[]){
		(struct service){361, IPPROTO_TCP, SOCK_STREAM},
		(struct service){361, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(262, "srssend", (struct service[]){
		(struct service){362, IPPROTO_TCP, SOCK_STREAM},
		(struct service){362, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(263, "rsvp-tunnel", (struct service[]){
		(struct service){363, IPPROTO_TCP, SOCK_STREAM},
		(struct service){363, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(264, "aurora-cmgr", (struct service[]){
		(struct service){364, IPPROTO_TCP, SOCK_STREAM},
		(struct service){364, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(265, "dtk", (struct service[]){
		(struct service){365, IPPROTO_TCP, SOCK_STREAM},
		(struct service){365, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(266, "odmr", (struct service[]){
		(struct service){366, IPPROTO_TCP, SOCK_STREAM},
		(struct service){366, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(267, "mortgageware", (struct service[]){
		(struct service){367, IPPROTO_TCP, SOCK_STREAM},
		(struct service){367, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(268, "qbikgdp", (struct service[]){
		(struct service){368, IPPROTO_TCP, SOCK_STREAM},
		(struct service){368, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(269, "rpc2portmap", (struct service[]){
		(struct service){369, IPPROTO_TCP, SOCK_STREAM},
		(struct service){369, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(270, "codaauth2", (struct service[]){
		(struct service){370, IPPROTO_TCP, SOCK_STREAM},
		(struct service){370, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(271, "clearcase", (struct service[]){
		(struct service){371, IPPROTO_TCP, SOCK_STREAM},
		(struct service){371, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(272, "ulistproc", (struct service[]){
		(struct service){372, IPPROTO_TCP, SOCK_STREAM},
		(struct service){372, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(273, "legent-1", (struct service[]){
		(struct service){373, IPPROTO_TCP, SOCK_STREAM},
		(struct service){373, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(274, "legent-2", (struct service[]){
		(struct service){374, IPPROTO_TCP, SOCK_STREAM},
		(struct service){374, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(275, "hassle", (struct service[]){
		(struct service){375, IPPROTO_TCP, SOCK_STREAM},
		(struct service){375, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(276, "nip", (struct service[]){
		(struct service){376, IPPROTO_TCP, SOCK_STREAM},
		(struct service){376, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(277, "tnETOS", (struct service[]){
		(struct service){377, IPPROTO_TCP, SOCK_STREAM},
		(struct service){377, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(278, "dsETOS", (struct service[]){
		(struct service){378, IPPROTO_TCP, SOCK_STREAM},
		(struct service){378, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(279, "is99c", (struct service[]){
		(struct service){379, IPPROTO_TCP, SOCK_STREAM},
		(struct service){379, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(280, "is99s", (struct service[]){
		(struct service){380, IPPROTO_TCP, SOCK_STREAM},
		(struct service){380, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(281, "hp-collector", (struct service[]){
		(struct service){381, IPPROTO_TCP, SOCK_STREAM},
		(struct service){381, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(282, "hp-managed-node", (struct service[]){
		(struct service){382, IPPROTO_TCP, SOCK_STREAM},
		(struct service){382, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(283, "hp-alarm-mgr", (struct service[]){
		(struct service){383, IPPROTO_TCP, SOCK_STREAM},
		(struct service){383, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(284, "arns", (struct service[]){
		(struct service){384, IPPROTO_TCP, SOCK_STREAM},
		(struct service){384, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(285, "ibm-app", (struct service[]){
		(struct service){385, IPPROTO_TCP, SOCK_STREAM},
		(struct service){385, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(286, "asa", (struct service[]){
		(struct service){386, IPPROTO_TCP, SOCK_STREAM},
		(struct service){386, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(287, "aurp", (struct service[]){
		(struct service){387, IPPROTO_TCP, SOCK_STREAM},
		(struct service){387, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(288, "unidata-ldm", (struct service[]){
		(struct service){388, IPPROTO_TCP, SOCK_STREAM},
		(struct service){388, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(289, "ldap", (struct service[]){
		(struct service){389, IPPROTO_TCP, SOCK_STREAM},
		(struct service){389, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(290, "uis", (struct service[]){
		(struct service){390, IPPROTO_TCP, SOCK_STREAM},
		(struct service){390, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(291, "synotics-relay", (struct service[]){
		(struct service){391, IPPROTO_TCP, SOCK_STREAM},
		(struct service){391, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(292, "synotics-broker", (struct service[]){
		(struct service){392, IPPROTO_TCP, SOCK_STREAM},
		(struct service){392, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(293, "meta5", (struct service[]){
		(struct service){393, IPPROTO_TCP, SOCK_STREAM},
		(struct service){393, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(294, "embl-ndt", (struct service[]){
		(struct service){394, IPPROTO_TCP, SOCK_STREAM},
		(struct service){394, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(295, "netcp", (struct service[]){
		(struct service){395, IPPROTO_TCP, SOCK_STREAM},
		(struct service){395, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(296, "netware-ip", (struct service[]){
		(struct service){396, IPPROTO_TCP, SOCK_STREAM},
		(struct service){396, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(297, "mptn", (struct service[]){
		(struct service){397, IPPROTO_TCP, SOCK_STREAM},
		(struct service){397, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(298, "kryptolan", (struct service[]){
		(struct service){398, IPPROTO_TCP, SOCK_STREAM},
		(struct service){398, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(299, "iso-tsap-c2", (struct service[]){
		(struct service){399, IPPROTO_TCP, SOCK_STREAM},
		(struct service){399, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(300, "osb-sd", (struct service[]){
		(struct service){400, IPPROTO_TCP, SOCK_STREAM},
		(struct service){400, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(301, "ups", (struct service[]){
		(struct service){401, IPPROTO_TCP, SOCK_STREAM},
		(struct service){401, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(302, "genie", (struct service[]){
		(struct service){402, IPPROTO_TCP, SOCK_STREAM},
		(struct service){402, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(303, "decap", (struct service[]){
		(struct service){403, IPPROTO_TCP, SOCK_STREAM},
		(struct service){403, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(304, "nced", (struct service[]){
		(struct service){404, IPPROTO_TCP, SOCK_STREAM},
		(struct service){404, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(305, "ncld", (struct service[]){
		(struct service){405, IPPROTO_TCP, SOCK_STREAM},
		(struct service){405, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(306, "imsp", (struct service[]){
		(struct service){406, IPPROTO_TCP, SOCK_STREAM},
		(struct service){406, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(307, "timbuktu", (struct service[]){
		(struct service){407, IPPROTO_TCP, SOCK_STREAM},
		(struct service){407, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(308, "prm-sm", (struct service[]){
		(struct service){408, IPPROTO_TCP, SOCK_STREAM},
		(struct service){408, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(309, "prm-nm", (struct service[]){
		(struct service){409, IPPROTO_TCP, SOCK_STREAM},
		(struct service){409, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(310, "decladebug", (struct service[]){
		(struct service){410, IPPROTO_TCP, SOCK_STREAM},
		(struct service){410, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(311, "rmt", (struct service[]){
		(struct service){411, IPPROTO_TCP, SOCK_STREAM},
		(struct service){411, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(312, "synoptics-trap", (struct service[]){
		(struct service){412, IPPROTO_TCP, SOCK_STREAM},
		(struct service){412, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(313, "smsp", (struct service[]){
		(struct service){413, IPPROTO_TCP, SOCK_STREAM},
		(struct service){413, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(314, "infoseek", (struct service[]){
		(struct service){414, IPPROTO_TCP, SOCK_STREAM},
		(struct service){414, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(315, "bnet", (struct service[]){
		(struct service){415, IPPROTO_TCP, SOCK_STREAM},
		(struct service){415, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(316, "silverplatter", (struct service[]){
		(struct service){416, IPPROTO_TCP, SOCK_STREAM},
		(struct service){416, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(317, "onmux", (struct service[]){
		(struct service){417, IPPROTO_TCP, SOCK_STREAM},
		(struct service){417, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(318, "hyper-g", (struct service[]){
		(struct service){418, IPPROTO_TCP, SOCK_STREAM},
		(struct service){418, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(319, "ariel1", (struct service[]){
		(struct service){419, IPPROTO_TCP, SOCK_STREAM},
		(struct service){419, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(320, "smpte", (struct service[]){
		(struct service){420, IPPROTO_TCP, SOCK_STREAM},
		(struct service){420, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(321, "ariel2", (struct service[]){
		(struct service){421, IPPROTO_TCP, SOCK_STREAM},
		(struct service){421, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(322, "ariel3", (struct service[]){
		(struct service){422, IPPROTO_TCP, SOCK_STREAM},
		(struct service){422, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(323, "opc-job-start", (struct service[]){
		(struct service){423, IPPROTO_TCP, SOCK_STREAM},
		(struct service){423, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(324, "opc-job-track", (struct service[]){
		(struct service){424, IPPROTO_TCP, SOCK_STREAM},
		(struct service){424, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(325, "icad-el", (struct service[]){
		(struct service){425, IPPROTO_TCP, SOCK_STREAM},
		(struct service){425, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(326, "smartsdp", (struct service[]){
		(struct service){426, IPPROTO_TCP, SOCK_STREAM},
		(struct service){426, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(327, "svrloc", (struct service[]){
		(struct service){427, IPPROTO_TCP, SOCK_STREAM},
		(struct service){427, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(328, "ocs-cmu", (struct service[]){
		(struct service){428, IPPROTO_TCP, SOCK_STREAM},
		(struct service){428, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(329, "ocs-amu", (struct service[]){
		(struct service){429, IPPROTO_TCP, SOCK_STREAM},
		(struct service){429, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(330, "utmpsd", (struct service[]){
		(struct service){430, IPPROTO_TCP, SOCK_STREAM},
		(struct service){430, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(331, "utmpcd", (struct service[]){
		(struct service){431, IPPROTO_TCP, SOCK_STREAM},
		(struct service){431, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(332, "iasd", (struct service[]){
		(struct service){432, IPPROTO_TCP, SOCK_STREAM},
		(struct service){432, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(333, "nnsp", (struct service[]){
		(struct service){433, IPPROTO_TCP, SOCK_STREAM},
		(struct service){433, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(334, "mobileip-agent", (struct service[]){
		(struct service){434, IPPROTO_TCP, SOCK_STREAM},
		(struct service){434, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(335, "mobilip-mn", (struct service[]){
		(struct service){435, IPPROTO_TCP, SOCK_STREAM},
		(struct service){435, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(336, "dna-cml", (struct service[]){
		(struct service){436, IPPROTO_TCP, SOCK_STREAM},
		(struct service){436, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(337, "comscm", (struct service[]){
		(struct service){437, IPPROTO_TCP, SOCK_STREAM},
		(struct service){437, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(338, "dsfgw", (struct service[]){
		(struct service){438, IPPROTO_TCP, SOCK_STREAM},
		(struct service){438, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(339, "dasp", (struct service[]){
		(struct service){439, IPPROTO_TCP, SOCK_STREAM},
		(struct service){439, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(340, "sgcp", (struct service[]){
		(struct service){440, IPPROTO_TCP, SOCK_STREAM},
		(struct service){440, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(341, "decvms-sysmgt", (struct service[]){
		(struct service){441, IPPROTO_TCP, SOCK_STREAM},
		(struct service){441, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(342, "cvc-hostd", (struct service[]){
		(struct service){442, IPPROTO_TCP, SOCK_STREAM},
		(struct service){442, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(343, "https", (struct service[]){
		(struct service){443, IPPROTO_TCP, SOCK_STREAM},
		(struct service){443, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(344, "snpp", (struct service[]){
		(struct service){444, IPPROTO_TCP, SOCK_STREAM},
		(struct service){444, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(345, "microsoft-ds", (struct service[]){
		(struct service){445, IPPROTO_TCP, SOCK_STREAM},
		(struct service){445, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(346, "ddm-rdb", (struct service[]){
		(struct service){446, IPPROTO_TCP, SOCK_STREAM},
		(struct service){446, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(347, "ddm-dfm", (struct service[]){
		(struct service){447, IPPROTO_TCP, SOCK_STREAM},
		(struct service){447, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(348, "ddm-ssl", (struct service[]){
		(struct service){448, IPPROTO_TCP, SOCK_STREAM},
		(struct service){448, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(349, "as-servermap", (struct service[]){
		(struct service){449, IPPROTO_TCP, SOCK_STREAM},
		(struct service){449, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(350, "tserver", (struct service[]){
		(struct service){450, IPPROTO_TCP, SOCK_STREAM},
		(struct service){450, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(351, "sfs-smp-net", (struct service[]){
		(struct service){451, IPPROTO_TCP, SOCK_STREAM},
		(struct service){451, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(352, "sfs-config", (struct service[]){
		(struct service){452, IPPROTO_TCP, SOCK_STREAM},
		(struct service){452, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(353, "creativeserver", (struct service[]){
		(struct service){453, IPPROTO_TCP, SOCK_STREAM},
		(struct service){453, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){3364, IPPROTO_TCP, SOCK_STREAM},
		(struct service){3364, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(354, "contentserver", (struct service[]){
		(struct service){454, IPPROTO_TCP, SOCK_STREAM},
		(struct service){454, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){3365, IPPROTO_TCP, SOCK_STREAM},
		(struct service){3365, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(355, "creativepartnr", (struct service[]){
		(struct service){455, IPPROTO_TCP, SOCK_STREAM},
		(struct service){455, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){3366, IPPROTO_TCP, SOCK_STREAM},
		(struct service){3366, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(356, "macon-tcp", (struct service[]){
		(struct service){456, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(357, "macon-udp", (struct service[]){
		(struct service){456, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(358, "scohelp", (struct service[]){
		(struct service){457, IPPROTO_TCP, SOCK_STREAM},
		(struct service){457, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(359, "appleqtc", (struct service[]){
		(struct service){458, IPPROTO_TCP, SOCK_STREAM},
		(struct service){458, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(360, "ampr-rcmd", (struct service[]){
		(struct service){459, IPPROTO_TCP, SOCK_STREAM},
		(struct service){459, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(361, "skronk", (struct service[]){
		(struct service){460, IPPROTO_TCP, SOCK_STREAM},
		(struct service){460, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(362, "datasurfsrv", (struct service[]){
		(struct service){461, IPPROTO_TCP, SOCK_STREAM},
		(struct service){461, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(363, "datasurfsrvsec", (struct service[]){
		(struct service){462, IPPROTO_TCP, SOCK_STREAM},
		(struct service){462, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(364, "alpes", (struct service[]){
		(struct service){463, IPPROTO_TCP, SOCK_STREAM},
		(struct service){463, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(365, "kpasswd", (struct service[]){
		(struct service){464, IPPROTO_TCP, SOCK_STREAM},
		(struct service){464, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(366, "urd", (struct service[]){
		(struct service){465, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(367, "submissions", (struct service[]){
		(struct service){465, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(368, "igmpv3lite", (struct service[]){
		(struct service){465, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(369, "digital-vrc", (struct service[]){
		(struct service){466, IPPROTO_TCP, SOCK_STREAM},
		(struct service){466, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(370, "mylex-mapd", (struct service[]){
		(struct service){467, IPPROTO_TCP, SOCK_STREAM},
		(struct service){467, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(371, "photuris", (struct service[]){
		(struct service){468, IPPROTO_TCP, SOCK_STREAM},
		(struct service){468, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(372, "rcp", (struct service[]){
		(struct service){469, IPPROTO_TCP, SOCK_STREAM},
		(struct service){469, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(373, "scx-proxy", (struct service[]){
		(struct service){470, IPPROTO_TCP, SOCK_STREAM},
		(struct service){470, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(374, "mondex", (struct service[]){
		(struct service){471, IPPROTO_TCP, SOCK_STREAM},
		(struct service){471, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(375, "ljk-login", (struct service[]){
		(struct service){472, IPPROTO_TCP, SOCK_STREAM},
		(struct service){472, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(376, "hybrid-pop", (struct service[]){
		(struct service){473, IPPROTO_TCP, SOCK_STREAM},
		(struct service){473, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(377, "tn-tl-w1", (struct service[]){
		(struct service){474, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(378, "tn-tl-w2", (struct service[]){
		(struct service){474, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(379, "tcpnethaspsrv", (struct service[]){
		(struct service){475, IPPROTO_TCP, SOCK_STREAM},
		(struct service){475, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(380, "tn-tl-fd1", (struct service[]){
		(struct service){476, IPPROTO_TCP, SOCK_STREAM},
		(struct service){476, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(381, "ss7ns", (struct service[]){
		(struct service){477, IPPROTO_TCP, SOCK_STREAM},
		(struct service){477, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(382, "spsc", (struct service[]){
		(struct service){478, IPPROTO_TCP, SOCK_STREAM},
		(struct service){478, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(383, "iafserver", (struct service[]){
		(struct service){479, IPPROTO_TCP, SOCK_STREAM},
		(struct service){479, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(384, "iafdbase", (struct service[]){
		(struct service){480, IPPROTO_TCP, SOCK_STREAM},
		(struct service){480, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(385, "ph", (struct service[]){
		(struct service){481, IPPROTO_TCP, SOCK_STREAM},
		(struct service){481, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(386, "bgs-nsi", (struct service[]){
		(struct service){482, IPPROTO_TCP, SOCK_STREAM},
		(struct service){482, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(387, "ulpnet", (struct service[]){
		(struct service){483, IPPROTO_TCP, SOCK_STREAM},
		(struct service){483, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(388, "integra-sme", (struct service[]){
		(struct service){484, IPPROTO_TCP, SOCK_STREAM},
		(struct service){484, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(389, "powerburst", (struct service[]){
		(struct service){485, IPPROTO_TCP, SOCK_STREAM},
		(struct service){485, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(390, "avian", (struct service[]){
		(struct service){486, IPPROTO_TCP, SOCK_STREAM},
		(struct service){486, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(391, "saft", (struct service[]){
		(struct service){487, IPPROTO_TCP, SOCK_STREAM},
		(struct service){487, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(392, "gss-http", (struct service[]){
		(struct service){488, IPPROTO_TCP, SOCK_STREAM},
		(struct service){488, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(393, "nest-protocol", (struct service[]){
		(struct service){489, IPPROTO_TCP, SOCK_STREAM},
		(struct service){489, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(394, "micom-pfs", (struct service[]){
		(struct service){490, IPPROTO_TCP, SOCK_STREAM},
		(struct service){490, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(395, "go-login", (struct service[]){
		(struct service){491, IPPROTO_TCP, SOCK_STREAM},
		(struct service){491, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(396, "ticf-1", (struct service[]){
		(struct service){492, IPPROTO_TCP, SOCK_STREAM},
		(struct service){492, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(397, "ticf-2", (struct service[]){
		(struct service){493, IPPROTO_TCP, SOCK_STREAM},
		(struct service){493, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(398, "pov-ray", (struct service[]){
		(struct service){494, IPPROTO_TCP, SOCK_STREAM},
		(struct service){494, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(399, "intecourier", (struct service[]){
		(struct service){495, IPPROTO_TCP, SOCK_STREAM},
		(struct service){495, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(400, "pim-rp-disc", (struct service[]){
		(struct service){496, IPPROTO_TCP, SOCK_STREAM},
		(struct service){496, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(401, "retrospect", (struct service[]){
		(struct service){497, IPPROTO_TCP, SOCK_STREAM},
		(struct service){497, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(402, "siam", (struct service[]){
		(struct service){498, IPPROTO_TCP, SOCK_STREAM},
		(struct service){498, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(403, "iso-ill", (struct service[]){
		(struct service){499, IPPROTO_TCP, SOCK_STREAM},
		(struct service){499, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(404, "isakmp", (struct service[]){
		(struct service){500, IPPROTO_TCP, SOCK_STREAM},
		(struct service){500, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(405, "stmf", (struct service[]){
		(struct service){501, IPPROTO_TCP, SOCK_STREAM},
		(struct service){501, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(406, "mbap", (struct service[]){
		(struct service){502, IPPROTO_TCP, SOCK_STREAM},
		(struct service){502, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(407, "intrinsa", (struct service[]){
		(struct service){503, IPPROTO_TCP, SOCK_STREAM},
		(struct service){503, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(408, "citadel", (struct service[]){
		(struct service){504, IPPROTO_TCP, SOCK_STREAM},
		(struct service){504, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(409, "mailbox-lm", (struct service[]){
		(struct service){505, IPPROTO_TCP, SOCK_STREAM},
		(struct service){505, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(410, "ohimsrv", (struct service[]){
		(struct service){506, IPPROTO_TCP, SOCK_STREAM},
		(struct service){506, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(411, "crs", (struct service[]){
		(struct service){507, IPPROTO_TCP, SOCK_STREAM},
		(struct service){507, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(412, "xvttp", (struct service[]){
		(struct service){508, IPPROTO_TCP, SOCK_STREAM},
		(struct service){508, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(413, "snare", (struct service[]){
		(struct service){509, IPPROTO_TCP, SOCK_STREAM},
		(struct service){509, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(414, "fcp", (struct service[]){
		(struct service){510, IPPROTO_TCP, SOCK_STREAM},
		(struct service){510, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(415, "passgo", (struct service[]){
		(struct service){511, IPPROTO_TCP, SOCK_STREAM},
		(struct service){511, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(416, "exec", (struct service[]){
		(struct service){512, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(417, "comsat", (struct service[]){
		(struct service){512, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(418, "biff", (struct service[]){
		(struct service){512, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(419, "login", (struct service[]){
		(struct service){513, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(420, "who", (struct service[]){
		(struct service){513, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(421, "shell", (struct service[]){
		(struct service){514, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(422, "syslog", (struct service[]){
		(struct service){514, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(423, "printer", (struct service[]){
		(struct service){515, IPPROTO_TCP, SOCK_STREAM},
		(struct service){515, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(424, "videotex", (struct service[]){
		(struct service){516, IPPROTO_TCP, SOCK_STREAM},
		(struct service){516, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(425, "talk", (struct service[]){
		(struct service){517, IPPROTO_TCP, SOCK_STREAM},
		(struct service){517, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(426, "ntalk", (struct service[]){
		(struct service){518, IPPROTO_TCP, SOCK_STREAM},
		(struct service){518, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(427, "utime", (struct service[]){
		(struct service){519, IPPROTO_TCP, SOCK_STREAM},
		(struct service){519, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(428, "efs", (struct service[]){
		(struct service){520, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(429, "router", (struct service[]){
		(struct service){520, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(430, "ripng", (struct service[]){
		(struct service){521, IPPROTO_TCP, SOCK_STREAM},
		(struct service){521, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(431, "ulp", (struct service[]){
		(struct service){522, IPPROTO_TCP, SOCK_STREAM},
		(struct service){522, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(432, "ibm-db2", (struct service[]){
		(struct service){523, IPPROTO_TCP, SOCK_STREAM},
		(struct service){523, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(433, "ncp", (struct service[]){
		(struct service){524, IPPROTO_TCP, SOCK_STREAM},
		(struct service){524, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(434, "timed", (struct service[]){
		(struct service){525, IPPROTO_TCP, SOCK_STREAM},
		(struct service){525, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(435, "tempo", (struct service[]){
		(struct service){526, IPPROTO_TCP, SOCK_STREAM},
		(struct service){526, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(436, "stx", (struct service[]){
		(struct service){527, IPPROTO_TCP, SOCK_STREAM},
		(struct service){527, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(437, "custix", (struct service[]){
		(struct service){528, IPPROTO_TCP, SOCK_STREAM},
		(struct service){528, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(438, "irc-serv", (struct service[]){
		(struct service){529, IPPROTO_TCP, SOCK_STREAM},
		(struct service){529, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(439, "courier", (struct service[]){
		(struct service){530, IPPROTO_TCP, SOCK_STREAM},
		(struct service){530, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(440, "conference", (struct service[]){
		(struct service){531, IPPROTO_TCP, SOCK_STREAM},
		(struct service){531, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(441, "netnews", (struct service[]){
		(struct service){532, IPPROTO_TCP, SOCK_STREAM},
		(struct service){532, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(442, "netwall", (struct service[]){
		(struct service){533, IPPROTO_TCP, SOCK_STREAM},
		(struct service){533, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(443, "windream", (struct service[]){
		(struct service){534, IPPROTO_TCP, SOCK_STREAM},
		(struct service){534, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(444, "iiop", (struct service[]){
		(struct service){535, IPPROTO_TCP, SOCK_STREAM},
		(struct service){535, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(445, "opalis-rdv", (struct service[]){
		(struct service){536, IPPROTO_TCP, SOCK_STREAM},
		(struct service){536, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(446, "nmsp", (struct service[]){
		(struct service){537, IPPROTO_TCP, SOCK_STREAM},
		(struct service){537, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){1790, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1790, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(447, "gdomap", (struct service[]){
		(struct service){538, IPPROTO_TCP, SOCK_STREAM},
		(struct service){538, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(448, "apertus-ldp", (struct service[]){
		(struct service){539, IPPROTO_TCP, SOCK_STREAM},
		(struct service){539, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(449, "uucp", (struct service[]){
		(struct service){540, IPPROTO_TCP, SOCK_STREAM},
		(struct service){540, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(450, "uucp-rlogin", (struct service[]){
		(struct service){541, IPPROTO_TCP, SOCK_STREAM},
		(struct service){541, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(451, "commerce", (struct service[]){
		(struct service){542, IPPROTO_TCP, SOCK_STREAM},
		(struct service){542, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(452, "klogin", (struct service[]){
		(struct service){543, IPPROTO_TCP, SOCK_STREAM},
		(struct service){543, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(453, "kshell", (struct service[]){
		(struct service){544, IPPROTO_TCP, SOCK_STREAM},
		(struct service){544, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(454, "appleqtcsrvr", (struct service[]){
		(struct service){545, IPPROTO_TCP, SOCK_STREAM},
		(struct service){545, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(455, "dhcpv6-client", (struct service[]){
		(struct service){546, IPPROTO_TCP, SOCK_STREAM},
		(struct service){546, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(456, "dhcpv6-server", (struct service[]){
		(struct service){547, IPPROTO_TCP, SOCK_STREAM},
		(struct service){547, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(457, "afpovertcp", (struct service[]){
		(struct service){548, IPPROTO_TCP, SOCK_STREAM},
		(struct service){548, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(458, "idfp", (struct service[]){
		(struct service){549, IPPROTO_TCP, SOCK_STREAM},
		(struct service){549, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(459, "new-rwho", (struct service[]){
		(struct service){550, IPPROTO_TCP, SOCK_STREAM},
		(struct service){550, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(460, "cybercash", (struct service[]){
		(struct service){551, IPPROTO_TCP, SOCK_STREAM},
		(struct service){551, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(461, "devshr-nts", (struct service[]){
		(struct service){552, IPPROTO_TCP, SOCK_STREAM},
		(struct service){552, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(462, "pirp", (struct service[]){
		(struct service){553, IPPROTO_TCP, SOCK_STREAM},
		(struct service){553, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(463, "rtsp", (struct service[]){
		(struct service){554, IPPROTO_TCP, SOCK_STREAM},
		(struct service){554, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(464, "dsf", (struct service[]){
		(struct service){555, IPPROTO_TCP, SOCK_STREAM},
		(struct service){555, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(465, "remotefs", (struct service[]){
		(struct service){556, IPPROTO_TCP, SOCK_STREAM},
		(struct service){556, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(466, "openvms-sysipc", (struct service[]){
		(struct service){557, IPPROTO_TCP, SOCK_STREAM},
		(struct service){557, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(467, "sdnskmp", (struct service[]){
		(struct service){558, IPPROTO_TCP, SOCK_STREAM},
		(struct service){558, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(468, "teedtap", (struct service[]){
		(struct service){559, IPPROTO_TCP, SOCK_STREAM},
		(struct service){559, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(469, "rmonitor", (struct service[]){
		(struct service){560, IPPROTO_TCP, SOCK_STREAM},
		(struct service){560, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(470, "monitor", (struct service[]){
		(struct service){561, IPPROTO_TCP, SOCK_STREAM},
		(struct service){561, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(471, "chshell", (struct service[]){
		(struct service){562, IPPROTO_TCP, SOCK_STREAM},
		(struct service){562, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(472, "nntps", (struct service[]){
		(struct service){563, IPPROTO_TCP, SOCK_STREAM},
		(struct service){563, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(473, "9pfs", (struct service[]){
		(struct service){564, IPPROTO_TCP, SOCK_STREAM},
		(struct service){564, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(474, "whoami", (struct service[]){
		(struct service){565, IPPROTO_TCP, SOCK_STREAM},
		(struct service){565, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(475, "streettalk", (struct service[]){
		(struct service){566, IPPROTO_TCP, SOCK_STREAM},
		(struct service){566, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(476, "banyan-rpc", (struct service[]){
		(struct service){567, IPPROTO_TCP, SOCK_STREAM},
		(struct service){567, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(477, "ms-shuttle", (struct service[]){
		(struct service){568, IPPROTO_TCP, SOCK_STREAM},
		(struct service){568, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(478, "ms-rome", (struct service[]){
		(struct service){569, IPPROTO_TCP, SOCK_STREAM},
		(struct service){569, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(479, "meter", (struct service[]){
		(struct service){570, IPPROTO_TCP, SOCK_STREAM},
		(struct service){570, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){571, IPPROTO_TCP, SOCK_STREAM},
		(struct service){571, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(480, "sonar", (struct service[]){
		(struct service){572, IPPROTO_TCP, SOCK_STREAM},
		(struct service){572, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(481, "banyan-vip", (struct service[]){
		(struct service){573, IPPROTO_TCP, SOCK_STREAM},
		(struct service){573, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(482, "ftp-agent", (struct service[]){
		(struct service){574, IPPROTO_TCP, SOCK_STREAM},
		(struct service){574, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(483, "vemmi", (struct service[]){
		(struct service){575, IPPROTO_TCP, SOCK_STREAM},
		(struct service){575, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(484, "ipcd", (struct service[]){
		(struct service){576, IPPROTO_TCP, SOCK_STREAM},
		(struct service){576, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(485, "vnas", (struct service[]){
		(struct service){577, IPPROTO_TCP, SOCK_STREAM},
		(struct service){577, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(486, "ipdd", (struct service[]){
		(struct service){578, IPPROTO_TCP, SOCK_STREAM},
		(struct service){578, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(487, "decbsrv", (struct service[]){
		(struct service){579, IPPROTO_TCP, SOCK_STREAM},
		(struct service){579, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(488, "sntp-heartbeat", (struct service[]){
		(struct service){580, IPPROTO_TCP, SOCK_STREAM},
		(struct service){580, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(489, "bdp", (struct service[]){
		(struct service){581, IPPROTO_TCP, SOCK_STREAM},
		(struct service){581, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(490, "scc-security", (struct service[]){
		(struct service){582, IPPROTO_TCP, SOCK_STREAM},
		(struct service){582, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(491, "philips-vc", (struct service[]){
		(struct service){583, IPPROTO_TCP, SOCK_STREAM},
		(struct service){583, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(492, "keyserver", (struct service[]){
		(struct service){584, IPPROTO_TCP, SOCK_STREAM},
		(struct service){584, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(493, "password-chg", (struct service[]){
		(struct service){586, IPPROTO_TCP, SOCK_STREAM},
		(struct service){586, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(494, "submission", (struct service[]){
		(struct service){587, IPPROTO_TCP, SOCK_STREAM},
		(struct service){587, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(495, "cal", (struct service[]){
		(struct service){588, IPPROTO_TCP, SOCK_STREAM},
		(struct service){588, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(496, "eyelink", (struct service[]){
		(struct service){589, IPPROTO_TCP, SOCK_STREAM},
		(struct service){589, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(497, "tns-cml", (struct service[]){
		(struct service){590, IPPROTO_TCP, SOCK_STREAM},
		(struct service){590, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(498, "http-alt", (struct service[]){
		(struct service){591, IPPROTO_TCP, SOCK_STREAM},
		(struct service){591, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){8008, IPPROTO_TCP, SOCK_STREAM},
		(struct service){8008, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){8080, IPPROTO_TCP, SOCK_STREAM},
		(struct service){8080, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(499, "eudora-set", (struct service[]){
		(struct service){592, IPPROTO_TCP, SOCK_STREAM},
		(struct service){592, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(500, "http-rpc-epmap", (struct service[]){
		(struct service){593, IPPROTO_TCP, SOCK_STREAM},
		(struct service){593, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(501, "tpip", (struct service[]){
		(struct service){594, IPPROTO_TCP, SOCK_STREAM},
		(struct service){594, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(502, "cab-protocol", (struct service[]){
		(struct service){595, IPPROTO_TCP, SOCK_STREAM},
		(struct service){595, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(503, "smsd", (struct service[]){
		(struct service){596, IPPROTO_TCP, SOCK_STREAM},
		(struct service){596, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(504, "ptcnameservice", (struct service[]){
		(struct service){597, IPPROTO_TCP, SOCK_STREAM},
		(struct service){597, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(505, "sco-websrvrmg3", (struct service[]){
		(struct service){598, IPPROTO_TCP, SOCK_STREAM},
		(struct service){598, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(506, "acp", (struct service[]){
		(struct service){599, IPPROTO_TCP, SOCK_STREAM},
		(struct service){599, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(507, "ipcserver", (struct service[]){
		(struct service){600, IPPROTO_TCP, SOCK_STREAM},
		(struct service){600, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(508, "syslog-conn", (struct service[]){
		(struct service){601, IPPROTO_TCP, SOCK_STREAM},
		(struct service){601, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(509, "xmlrpc-beep", (struct service[]){
		(struct service){602, IPPROTO_TCP, SOCK_STREAM},
		(struct service){602, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(510, "idxp", (struct service[]){
		(struct service){603, IPPROTO_TCP, SOCK_STREAM},
		(struct service){603, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(511, "tunnel", (struct service[]){
		(struct service){604, IPPROTO_TCP, SOCK_STREAM},
		(struct service){604, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(512, "soap-beep", (struct service[]){
		(struct service){605, IPPROTO_TCP, SOCK_STREAM},
		(struct service){605, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(513, "urm", (struct service[]){
		(struct service){606, IPPROTO_TCP, SOCK_STREAM},
		(struct service){606, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(514, "nqs", (struct service[]){
		(struct service){607, IPPROTO_TCP, SOCK_STREAM},
		(struct service){607, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(515, "sift-uft", (struct service[]){
		(struct service){608, IPPROTO_TCP, SOCK_STREAM},
		(struct service){608, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(516, "npmp-trap", (struct service[]){
		(struct service){609, IPPROTO_TCP, SOCK_STREAM},
		(struct service){609, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(517, "npmp-local", (struct service[]){
		(struct service){610, IPPROTO_TCP, SOCK_STREAM},
		(struct service){610, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(518, "npmp-gui", (struct service[]){
		(struct service){611, IPPROTO_TCP, SOCK_STREAM},
		(struct service){611, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(519, "hmmp-ind", (struct service[]){
		(struct service){612, IPPROTO_TCP, SOCK_STREAM},
		(struct service){612, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(520, "hmmp-op", (struct service[]){
		(struct service){613, IPPROTO_TCP, SOCK_STREAM},
		(struct service){613, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(521, "sshell", (struct service[]){
		(struct service){614, IPPROTO_TCP, SOCK_STREAM},
		(struct service){614, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(522, "sco-inetmgr", (struct service[]){
		(struct service){615, IPPROTO_TCP, SOCK_STREAM},
		(struct service){615, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(523, "sco-sysmgr", (struct service[]){
		(struct service){616, IPPROTO_TCP, SOCK_STREAM},
		(struct service){616, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(524, "sco-dtmgr", (struct service[]){
		(struct service){617, IPPROTO_TCP, SOCK_STREAM},
		(struct service){617, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(525, "dei-icda", (struct service[]){
		(struct service){618, IPPROTO_TCP, SOCK_STREAM},
		(struct service){618, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(526, "compaq-evm", (struct service[]){
		(struct service){619, IPPROTO_TCP, SOCK_STREAM},
		(struct service){619, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(527, "sco-websrvrmgr", (struct service[]){
		(struct service){620, IPPROTO_TCP, SOCK_STREAM},
		(struct service){620, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(528, "escp-ip", (struct service[]){
		(struct service){621, IPPROTO_TCP, SOCK_STREAM},
		(struct service){621, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(529, "collaborator", (struct service[]){
		(struct service){622, IPPROTO_TCP, SOCK_STREAM},
		(struct service){622, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(530, "oob-ws-http", (struct service[]){
		(struct service){623, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(531, "asf-rmcp", (struct service[]){
		(struct service){623, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(532, "cryptoadmin", (struct service[]){
		(struct service){624, IPPROTO_TCP, SOCK_STREAM},
		(struct service){624, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(533, "dec-dlm", (struct service[]){
		(struct service){625, IPPROTO_TCP, SOCK_STREAM},
		(struct service){625, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(534, "asia", (struct service[]){
		(struct service){626, IPPROTO_TCP, SOCK_STREAM},
		(struct service){626, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(535, "passgo-tivoli", (struct service[]){
		(struct service){627, IPPROTO_TCP, SOCK_STREAM},
		(struct service){627, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(536, "qmqp", (struct service[]){
		(struct service){628, IPPROTO_TCP, SOCK_STREAM},
		(struct service){628, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(537, "3com-amp3", (struct service[]){
		(struct service){629, IPPROTO_TCP, SOCK_STREAM},
		(struct service){629, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(538, "rda", (struct service[]){
		(struct service){630, IPPROTO_TCP, SOCK_STREAM},
		(struct service){630, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){2371, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(539, "ipp", (struct service[]){
		(struct service){631, IPPROTO_TCP, SOCK_STREAM},
		(struct service){631, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(540, "ipps", (struct service[]){
		(struct service){631, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(541, "bmpp", (struct service[]){
		(struct service){632, IPPROTO_TCP, SOCK_STREAM},
		(struct service){632, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(542, "servstat", (struct service[]){
		(struct service){633, IPPROTO_TCP, SOCK_STREAM},
		(struct service){633, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(543, "ginad", (struct service[]){
		(struct service){634, IPPROTO_TCP, SOCK_STREAM},
		(struct service){634, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(544, "rlzdbase", (struct service[]){
		(struct service){635, IPPROTO_TCP, SOCK_STREAM},
		(struct service){635, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(545, "ldaps", (struct service[]){
		(struct service){636, IPPROTO_TCP, SOCK_STREAM},
		(struct service){636, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(546, "lanserver", (struct service[]){
		(struct service){637, IPPROTO_TCP, SOCK_STREAM},
		(struct service){637, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(547, "mcns-sec", (struct service[]){
		(struct service){638, IPPROTO_TCP, SOCK_STREAM},
		(struct service){638, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(548, "msdp", (struct service[]){
		(struct service){639, IPPROTO_TCP, SOCK_STREAM},
		(struct service){639, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(549, "entrust-sps", (struct service[]){
		(struct service){640, IPPROTO_TCP, SOCK_STREAM},
		(struct service){640, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(550, "repcmd", (struct service[]){
		(struct service){641, IPPROTO_TCP, SOCK_STREAM},
		(struct service){641, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(551, "esro-emsdp", (struct service[]){
		(struct service){642, IPPROTO_TCP, SOCK_STREAM},
		(struct service){642, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(552, "sanity", (struct service[]){
		(struct service){643, IPPROTO_TCP, SOCK_STREAM},
		(struct service){643, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(553, "dwr", (struct service[]){
		(struct service){644, IPPROTO_TCP, SOCK_STREAM},
		(struct service){644, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(554, "pssc", (struct service[]){
		(struct service){645, IPPROTO_TCP, SOCK_STREAM},
		(struct service){645, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(555, "ldp", (struct service[]){
		(struct service){646, IPPROTO_TCP, SOCK_STREAM},
		(struct service){646, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(556, "dhcp-failover", (struct service[]){
		(struct service){647, IPPROTO_TCP, SOCK_STREAM},
		(struct service){647, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(557, "rrp", (struct service[]){
		(struct service){648, IPPROTO_TCP, SOCK_STREAM},
		(struct service){648, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(558, "cadview-3d", (struct service[]){
		(struct service){649, IPPROTO_TCP, SOCK_STREAM},
		(struct service){649, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(559, "obex", (struct service[]){
		(struct service){650, IPPROTO_TCP, SOCK_STREAM},
		(struct service){650, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(560, "ieee-mms", (struct service[]){
		(struct service){651, IPPROTO_TCP, SOCK_STREAM},
		(struct service){651, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(561, "hello-port", (struct service[]){
		(struct service){652, IPPROTO_TCP, SOCK_STREAM},
		(struct service){652, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(562, "repscmd", (struct service[]){
		(struct service){653, IPPROTO_TCP, SOCK_STREAM},
		(struct service){653, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(563, "aodv", (struct service[]){
		(struct service){654, IPPROTO_TCP, SOCK_STREAM},
		(struct service){654, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(564, "tinc", (struct service[]){
		(struct service){655, IPPROTO_TCP, SOCK_STREAM},
		(struct service){655, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(565, "spmp", (struct service[]){
		(struct service){656, IPPROTO_TCP, SOCK_STREAM},
		(struct service){656, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(566, "rmc", (struct service[]){
		(struct service){657, IPPROTO_TCP, SOCK_STREAM},
		(struct service){657, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(567, "tenfold", (struct service[]){
		(struct service){658, IPPROTO_TCP, SOCK_STREAM},
		(struct service){658, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(568, "mac-srvr-admin", (struct service[]){
		(struct service){660, IPPROTO_TCP, SOCK_STREAM},
		(struct service){660, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(569, "hap", (struct service[]){
		(struct service){661, IPPROTO_TCP, SOCK_STREAM},
		(struct service){661, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(570, "pftp", (struct service[]){
		(struct service){662, IPPROTO_TCP, SOCK_STREAM},
		(struct service){662, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(571, "purenoise", (struct service[]){
		(struct service){663, IPPROTO_TCP, SOCK_STREAM},
		(struct service){663, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(572, "oob-ws-https", (struct service[]){
		(struct service){664, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(573, "asf-secure-rmcp", (struct service[]){
		(struct service){664, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(574, "sun-dr", (struct service[]){
		(struct service){665, IPPROTO_TCP, SOCK_STREAM},
		(struct service){665, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(575, "mdqs", (struct service[]){
		(struct service){666, IPPROTO_TCP, SOCK_STREAM},
		(struct service){666, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(576, "doom", (struct service[]){
		(struct service){666, IPPROTO_TCP, SOCK_STREAM},
		(struct service){666, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(577, "disclose", (struct service[]){
		(struct service){667, IPPROTO_TCP, SOCK_STREAM},
		(struct service){667, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(578, "mecomm", (struct service[]){
		(struct service){668, IPPROTO_TCP, SOCK_STREAM},
		(struct service){668, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(579, "meregister", (struct service[]){
		(struct service){669, IPPROTO_TCP, SOCK_STREAM},
		(struct service){669, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(580, "vacdsm-sws", (struct service[]){
		(struct service){670, IPPROTO_TCP, SOCK_STREAM},
		(struct service){670, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(581, "vacdsm-app", (struct service[]){
		(struct service){671, IPPROTO_TCP, SOCK_STREAM},
		(struct service){671, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(582, "vpps-qua", (struct service[]){
		(struct service){672, IPPROTO_TCP, SOCK_STREAM},
		(struct service){672, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(583, "cimplex", (struct service[]){
		(struct service){673, IPPROTO_TCP, SOCK_STREAM},
		(struct service){673, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(584, "acap", (struct service[]){
		(struct service){674, IPPROTO_TCP, SOCK_STREAM},
		(struct service){674, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(585, "dctp", (struct service[]){
		(struct service){675, IPPROTO_TCP, SOCK_STREAM},
		(struct service){675, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(586, "vpps-via", (struct service[]){
		(struct service){676, IPPROTO_TCP, SOCK_STREAM},
		(struct service){676, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(587, "vpp", (struct service[]){
		(struct service){677, IPPROTO_TCP, SOCK_STREAM},
		(struct service){677, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(588, "ggf-ncp", (struct service[]){
		(struct service){678, IPPROTO_TCP, SOCK_STREAM},
		(struct service){678, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(589, "mrm", (struct service[]){
		(struct service){679, IPPROTO_TCP, SOCK_STREAM},
		(struct service){679, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(590, "entrust-aaas", (struct service[]){
		(struct service){680, IPPROTO_TCP, SOCK_STREAM},
		(struct service){680, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(591, "entrust-aams", (struct service[]){
		(struct service){681, IPPROTO_TCP, SOCK_STREAM},
		(struct service){681, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(592, "xfr", (struct service[]){
		(struct service){682, IPPROTO_TCP, SOCK_STREAM},
		(struct service){682, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(593, "corba-iiop", (struct service[]){
		(struct service){683, IPPROTO_TCP, SOCK_STREAM},
		(struct service){683, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(594, "corba-iiop-ssl", (struct service[]){
		(struct service){684, IPPROTO_TCP, SOCK_STREAM},
		(struct service){684, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(595, "mdc-portmapper", (struct service[]){
		(struct service){685, IPPROTO_TCP, SOCK_STREAM},
		(struct service){685, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(596, "hcp-wismar", (struct service[]){
		(struct service){686, IPPROTO_TCP, SOCK_STREAM},
		(struct service){686, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(597, "asipregistry", (struct service[]){
		(struct service){687, IPPROTO_TCP, SOCK_STREAM},
		(struct service){687, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(598, "realm-rusd", (struct service[]){
		(struct service){688, IPPROTO_TCP, SOCK_STREAM},
		(struct service){688, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(599, "nmap", (struct service[]){
		(struct service){689, IPPROTO_TCP, SOCK_STREAM},
		(struct service){689, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(600, "vatp", (struct service[]){
		(struct service){690, IPPROTO_TCP, SOCK_STREAM},
		(struct service){690, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(601, "msexch-routing", (struct service[]){
		(struct service){691, IPPROTO_TCP, SOCK_STREAM},
		(struct service){691, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(602, "hyperwave-isp", (struct service[]){
		(struct service){692, IPPROTO_TCP, SOCK_STREAM},
		(struct service){692, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(603, "connendp", (struct service[]){
		(struct service){693, IPPROTO_TCP, SOCK_STREAM},
		(struct service){693, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(604, "ha-cluster", (struct service[]){
		(struct service){694, IPPROTO_TCP, SOCK_STREAM},
		(struct service){694, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(605, "ieee-mms-ssl", (struct service[]){
		(struct service){695, IPPROTO_TCP, SOCK_STREAM},
		(struct service){695, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(606, "rushd", (struct service[]){
		(struct service){696, IPPROTO_TCP, SOCK_STREAM},
		(struct service){696, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(607, "uuidgen", (struct service[]){
		(struct service){697, IPPROTO_TCP, SOCK_STREAM},
		(struct service){697, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(608, "olsr", (struct service[]){
		(struct service){698, IPPROTO_TCP, SOCK_STREAM},
		(struct service){698, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(609, "accessnetwork", (struct service[]){
		(struct service){699, IPPROTO_TCP, SOCK_STREAM},
		(struct service){699, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(610, "epp", (struct service[]){
		(struct service){700, IPPROTO_TCP, SOCK_STREAM},
		(struct service){700, IPPROTO_UDP, SOCK_DGRAM},
		(struct service){3044, IPPROTO_TCP, SOCK_STREAM},
		(struct service){3044, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(611, "lmp", (struct service[]){
		(struct service){701, IPPROTO_TCP, SOCK_STREAM},
		(struct service){701, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(612, "iris-beep", (struct service[]){
		(struct service){702, IPPROTO_TCP, SOCK_STREAM},
		(struct service){702, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(613, "elcsd", (struct service[]){
		(struct service){704, IPPROTO_TCP, SOCK_STREAM},
		(struct service){704, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(614, "agentx", (struct service[]){
		(struct service){705, IPPROTO_TCP, SOCK_STREAM},
		(struct service){705, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(615, "silc", (struct service[]){
		(struct service){706, IPPROTO_TCP, SOCK_STREAM},
		(struct service){706, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(616, "borland-dsj", (struct service[]){
		(struct service){707, IPPROTO_TCP, SOCK_STREAM},
		(struct service){707, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(617, "entrust-kmsh", (struct service[]){
		(struct service){709, IPPROTO_TCP, SOCK_STREAM},
		(struct service){709, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(618, "entrust-ash", (struct service[]){
		(struct service){710, IPPROTO_TCP, SOCK_STREAM},
		(struct service){710, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(619, "cisco-tdp", (struct service[]){
		(struct service){711, IPPROTO_TCP, SOCK_STREAM},
		(struct service){711, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(620, "tbrpf", (struct service[]){
		(struct service){712, IPPROTO_TCP, SOCK_STREAM},
		(struct service){712, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(621, "iris-xpc", (struct service[]){
		(struct service){713, IPPROTO_TCP, SOCK_STREAM},
		(struct service){713, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(622, "iris-xpcs", (struct service[]){
		(struct service){714, IPPROTO_TCP, SOCK_STREAM},
		(struct service){714, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(623, "iris-lwz", (struct service[]){
		(struct service){715, IPPROTO_TCP, SOCK_STREAM},
		(struct service){715, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(624, "pana", (struct service[]){
		(struct service){716, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(625, "netviewdm1", (struct service[]){
		(struct service){729, IPPROTO_TCP, SOCK_STREAM},
		(struct service){729, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(626, "netviewdm2", (struct service[]){
		(struct service){730, IPPROTO_TCP, SOCK_STREAM},
		(struct service){730, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(627, "netviewdm3", (struct service[]){
		(struct service){731, IPPROTO_TCP, SOCK_STREAM},
		(struct service){731, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(628, "netgw", (struct service[]){
		(struct service){741, IPPROTO_TCP, SOCK_STREAM},
		(struct service){741, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(629, "netrcs", (struct service[]){
		(struct service){742, IPPROTO_TCP, SOCK_STREAM},
		(struct service){742, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(630, "flexlm", (struct service[]){
		(struct service){744, IPPROTO_TCP, SOCK_STREAM},
		(struct service){744, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(631, "fujitsu-dev", (struct service[]){
		(struct service){747, IPPROTO_TCP, SOCK_STREAM},
		(struct service){747, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(632, "ris-cm", (struct service[]){
		(struct service){748, IPPROTO_TCP, SOCK_STREAM},
		(struct service){748, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(633, "kerberos-adm", (struct service[]){
		(struct service){749, IPPROTO_TCP, SOCK_STREAM},
		(struct service){749, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(634, "rfile", (struct service[]){
		(struct service){750, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(635, "loadav", (struct service[]){
		(struct service){750, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(636, "kerberos-iv", (struct service[]){
		(struct service){750, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(637, "pump", (struct service[]){
		(struct service){751, IPPROTO_TCP, SOCK_STREAM},
		(struct service){751, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(638, "qrh", (struct service[]){
		(struct service){752, IPPROTO_TCP, SOCK_STREAM},
		(struct service){752, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(639, "rrh", (struct service[]){
		(struct service){753, IPPROTO_TCP, SOCK_STREAM},
		(struct service){753, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(640, "tell", (struct service[]){
		(struct service){754, IPPROTO_TCP, SOCK_STREAM},
		(struct service){754, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(641, "nlogin", (struct service[]){
		(struct service){758, IPPROTO_TCP, SOCK_STREAM},
		(struct service){758, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(642, "con", (struct service[]){
		(struct service){759, IPPROTO_TCP, SOCK_STREAM},
		(struct service){759, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(643, "ns", (struct service[]){
		(struct service){760, IPPROTO_TCP, SOCK_STREAM},
		(struct service){760, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(644, "rxe", (struct service[]){
		(struct service){761, IPPROTO_TCP, SOCK_STREAM},
		(struct service){761, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(645, "quotad", (struct service[]){
		(struct service){762, IPPROTO_TCP, SOCK_STREAM},
		(struct service){762, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(646, "cycleserv", (struct service[]){
		(struct service){763, IPPROTO_TCP, SOCK_STREAM},
		(struct service){763, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(647, "omserv", (struct service[]){
		(struct service){764, IPPROTO_TCP, SOCK_STREAM},
		(struct service){764, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(648, "webster", (struct service[]){
		(struct service){765, IPPROTO_TCP, SOCK_STREAM},
		(struct service){765, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(649, "phonebook", (struct service[]){
		(struct service){767, IPPROTO_TCP, SOCK_STREAM},
		(struct service){767, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(650, "vid", (struct service[]){
		(struct service){769, IPPROTO_TCP, SOCK_STREAM},
		(struct service){769, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(651, "cadlock", (struct service[]){
		(struct service){770, IPPROTO_TCP, SOCK_STREAM},
		(struct service){770, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(652, "rtip", (struct service[]){
		(struct service){771, IPPROTO_TCP, SOCK_STREAM},
		(struct service){771, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(653, "cycleserv2", (struct service[]){
		(struct service){772, IPPROTO_TCP, SOCK_STREAM},
		(struct service){772, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(654, "submit", (struct service[]){
		(struct service){773, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(655, "notify", (struct service[]){
		(struct service){773, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(656, "rpasswd", (struct service[]){
		(struct service){774, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(657, "acmaint-dbd", (struct service[]){
		(struct service){774, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(658, "entomb", (struct service[]){
		(struct service){775, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(659, "acmaint-transd", (struct service[]){
		(struct service){775, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(660, "wpages", (struct service[]){
		(struct service){776, IPPROTO_TCP, SOCK_STREAM},
		(struct service){776, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(661, "multiling-http", (struct service[]){
		(struct service){777, IPPROTO_TCP, SOCK_STREAM},
		(struct service){777, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(662, "wpgs", (struct service[]){
		(struct service){780, IPPROTO_TCP, SOCK_STREAM},
		(struct service){780, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(663, "mdbs-daemon", (struct service[]){
		(struct service){800, IPPROTO_TCP, SOCK_STREAM},
		(struct service){800, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(664, "device", (struct service[]){
		(struct service){801, IPPROTO_TCP, SOCK_STREAM},
		(struct service){801, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(665, "mbap-s", (struct service[]){
		(struct service){802, IPPROTO_TCP, SOCK_STREAM},
		(struct service){802, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(666, "fcp-udp", (struct service[]){
		(struct service){810, IPPROTO_TCP, SOCK_STREAM},
		(struct service){810, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(667, "itm-mcell-s", (struct service[]){
		(struct service){828, IPPROTO_TCP, SOCK_STREAM},
		(struct service){828, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(668, "pkix-3-ca-ra", (struct service[]){
		(struct service){829, IPPROTO_TCP, SOCK_STREAM},
		(struct service){829, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(669, "netconf-ssh", (struct service[]){
		(struct service){830, IPPROTO_TCP, SOCK_STREAM},
		(struct service){830, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(670, "netconf-beep", (struct service[]){
		(struct service){831, IPPROTO_TCP, SOCK_STREAM},
		(struct service){831, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(671, "netconfsoaphttp", (struct service[]){
		(struct service){832, IPPROTO_TCP, SOCK_STREAM},
		(struct service){832, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(672, "netconfsoapbeep", (struct service[]){
		(struct service){833, IPPROTO_TCP, SOCK_STREAM},
		(struct service){833, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(673, "dhcp-failover2", (struct service[]){
		(struct service){847, IPPROTO_TCP, SOCK_STREAM},
		(struct service){847, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(674, "gdoi", (struct service[]){
		(struct service){848, IPPROTO_TCP, SOCK_STREAM},
		(struct service){848, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(675, "domain-s", (struct service[]){
		(struct service){853, IPPROTO_TCP, SOCK_STREAM},
		(struct service){853, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(676, "dlep", (struct service[]){
		(struct service){854, IPPROTO_TCP, SOCK_STREAM},
		(struct service){854, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(677, "iscsi", (struct service[]){
		(struct service){860, IPPROTO_TCP, SOCK_STREAM},
		(struct service){860, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(678, "owamp-control", (struct service[]){
		(struct service){861, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(679, "owamp-test", (struct service[]){
		(struct service){861, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(680, "twamp-control", (struct service[]){
		(struct service){862, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(681, "twamp-test", (struct service[]){
		(struct service){862, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(682, "rsync", (struct service[]){
		(struct service){873, IPPROTO_TCP, SOCK_STREAM},
		(struct service){873, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(683, "iclcnet-locate", (struct service[]){
		(struct service){886, IPPROTO_TCP, SOCK_STREAM},
		(struct service){886, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(684, "iclcnet-svinfo", (struct service[]){
		(struct service){887, IPPROTO_TCP, SOCK_STREAM},
		(struct service){887, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(685, "accessbuilder", (struct service[]){
		(struct service){888, IPPROTO_TCP, SOCK_STREAM},
		(struct service){888, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(686, "cddbp", (struct service[]){
		(struct service){888, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(687, "omginitialrefs", (struct service[]){
		(struct service){900, IPPROTO_TCP, SOCK_STREAM},
		(struct service){900, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(688, "smpnameres", (struct service[]){
		(struct service){901, IPPROTO_TCP, SOCK_STREAM},
		(struct service){901, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(689, "ideafarm-door", (struct service[]){
		(struct service){902, IPPROTO_TCP, SOCK_STREAM},
		(struct service){902, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(690, "ideafarm-panic", (struct service[]){
		(struct service){903, IPPROTO_TCP, SOCK_STREAM},
		(struct service){903, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(691, "kink", (struct service[]){
		(struct service){910, IPPROTO_TCP, SOCK_STREAM},
		(struct service){910, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(692, "xact-backup", (struct service[]){
		(struct service){911, IPPROTO_TCP, SOCK_STREAM},
		(struct service){911, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(693, "apex-mesh", (struct service[]){
		(struct service){912, IPPROTO_TCP, SOCK_STREAM},
		(struct service){912, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(694, "apex-edge", (struct service[]){
		(struct service){913, IPPROTO_TCP, SOCK_STREAM},
		(struct service){913, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(695, "rift-lies", (struct service[]){
		(struct service){914, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(696, "rift-ties", (struct service[]){
		(struct service){915, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(697, "rndc", (struct service[]){
		(struct service){953, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(698, "ftps-data", (struct service[]){
		(struct service){989, IPPROTO_TCP, SOCK_STREAM},
		(struct service){989, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(699, "ftps", (struct service[]){
		(struct service){990, IPPROTO_TCP, SOCK_STREAM},
		(struct service){990, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(700, "nas", (struct service[]){
		(struct service){991, IPPROTO_TCP, SOCK_STREAM},
		(struct service){991, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(701, "telnets", (struct service[]){
		(struct service){992, IPPROTO_TCP, SOCK_STREAM},
		(struct service){992, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(702, "imaps", (struct service[]){
		(struct service){993, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(703, "pop3s", (struct service[]){
		(struct service){995, IPPROTO_TCP, SOCK_STREAM},
		(struct service){995, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(704, "vsinet", (struct service[]){
		(struct service){996, IPPROTO_TCP, SOCK_STREAM},
		(struct service){996, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(705, "maitrd", (struct service[]){
		(struct service){997, IPPROTO_TCP, SOCK_STREAM},
		(struct service){997, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(706, "busboy", (struct service[]){
		(struct service){998, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(707, "puparp", (struct service[]){
		(struct service){998, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(708, "garcon", (struct service[]){
		(struct service){999, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(709, "applix", (struct service[]){
		(struct service){999, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(710, "puprouter", (struct service[]){
		(struct service){999, IPPROTO_TCP, SOCK_STREAM},
		(struct service){999, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(711, "cadlock2", (struct service[]){
		(struct service){1000, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1000, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(712, "webpush", (struct service[]){
		(struct service){1001, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(713, "surf", (struct service[]){
		(struct service){1010, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1010, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(714, "exp1", (struct service[]){
		(struct service){1021, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1021, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(715, "exp2", (struct service[]){
		(struct service){1022, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1022, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(716, "blackjack", (struct service[]){
		(struct service){1025, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1025, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(717, "cap", (struct service[]){
		(struct service){1026, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1026, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(718, "6a44", (struct service[]){
		(struct service){1027, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(719, "solid-mux", (struct service[]){
		(struct service){1029, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1029, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(720, "netinfo-local", (struct service[]){
		(struct service){1033, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1033, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(721, "activesync", (struct service[]){
		(struct service){1034, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1034, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(722, "mxxrlogin", (struct service[]){
		(struct service){1035, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1035, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(723, "nsstp", (struct service[]){
		(struct service){1036, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1036, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(724, "ams", (struct service[]){
		(struct service){1037, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1037, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(725, "mtqp", (struct service[]){
		(struct service){1038, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1038, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(726, "sbl", (struct service[]){
		(struct service){1039, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1039, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(727, "netarx", (struct service[]){
		(struct service){1040, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1040, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(728, "danf-ak2", (struct service[]){
		(struct service){1041, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1041, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(729, "afrog", (struct service[]){
		(struct service){1042, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1042, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(730, "boinc-client", (struct service[]){
		(struct service){1043, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1043, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(731, "dcutility", (struct service[]){
		(struct service){1044, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1044, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(732, "fpitp", (struct service[]){
		(struct service){1045, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1045, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(733, "wfremotertm", (struct service[]){
		(struct service){1046, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1046, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(734, "neod1", (struct service[]){
		(struct service){1047, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1047, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(735, "neod2", (struct service[]){
		(struct service){1048, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1048, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(736, "td-postman", (struct service[]){
		(struct service){1049, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1049, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(737, "cma", (struct service[]){
		(struct service){1050, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1050, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(738, "optima-vnet", (struct service[]){
		(struct service){1051, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1051, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(739, "ddt", (struct service[]){
		(struct service){1052, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1052, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(740, "remote-as", (struct service[]){
		(struct service){1053, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1053, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(741, "brvread", (struct service[]){
		(struct service){1054, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1054, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(742, "ansyslmd", (struct service[]){
		(struct service){1055, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1055, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(743, "vfo", (struct service[]){
		(struct service){1056, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1056, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(744, "startron", (struct service[]){
		(struct service){1057, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1057, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(745, "nim", (struct service[]){
		(struct service){1058, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1058, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(746, "nimreg", (struct service[]){
		(struct service){1059, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1059, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(747, "polestar", (struct service[]){
		(struct service){1060, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1060, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(748, "kiosk", (struct service[]){
		(struct service){1061, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1061, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(749, "veracity", (struct service[]){
		(struct service){1062, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1062, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(750, "kyoceranetdev", (struct service[]){
		(struct service){1063, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1063, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(751, "jstel", (struct service[]){
		(struct service){1064, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1064, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(752, "syscomlan", (struct service[]){
		(struct service){1065, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1065, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(753, "fpo-fns", (struct service[]){
		(struct service){1066, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1066, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(754, "instl-boots", (struct service[]){
		(struct service){1067, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1067, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(755, "instl-bootc", (struct service[]){
		(struct service){1068, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1068, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(756, "cognex-insight", (struct service[]){
		(struct service){1069, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1069, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(757, "gmrupdateserv", (struct service[]){
		(struct service){1070, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1070, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(758, "bsquare-voip", (struct service[]){
		(struct service){1071, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1071, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(759, "cardax", (struct service[]){
		(struct service){1072, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1072, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(760, "bridgecontrol", (struct service[]){
		(struct service){1073, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1073, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(761, "warmspotMgmt", (struct service[]){
		(struct service){1074, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1074, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(762, "rdrmshc", (struct service[]){
		(struct service){1075, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1075, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(763, "dab-sti-c", (struct service[]){
		(struct service){1076, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1076, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(764, "imgames", (struct service[]){
		(struct service){1077, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1077, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(765, "avocent-proxy", (struct service[]){
		(struct service){1078, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1078, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(766, "asprovatalk", (struct service[]){
		(struct service){1079, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1079, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(767, "socks", (struct service[]){
		(struct service){1080, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1080, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(768, "pvuniwien", (struct service[]){
		(struct service){1081, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1081, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(769, "amt-esd-prot", (struct service[]){
		(struct service){1082, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1082, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(770, "ansoft-lm-1", (struct service[]){
		(struct service){1083, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1083, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(771, "ansoft-lm-2", (struct service[]){
		(struct service){1084, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1084, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(772, "webobjects", (struct service[]){
		(struct service){1085, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1085, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(773, "cplscrambler-lg", (struct service[]){
		(struct service){1086, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1086, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(774, "cplscrambler-in", (struct service[]){
		(struct service){1087, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1087, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(775, "cplscrambler-al", (struct service[]){
		(struct service){1088, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1088, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(776, "ff-annunc", (struct service[]){
		(struct service){1089, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1089, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(777, "ff-fms", (struct service[]){
		(struct service){1090, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1090, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(778, "ff-sm", (struct service[]){
		(struct service){1091, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1091, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(779, "obrpd", (struct service[]){
		(struct service){1092, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1092, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(780, "proofd", (struct service[]){
		(struct service){1093, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1093, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(781, "rootd", (struct service[]){
		(struct service){1094, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1094, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(782, "nicelink", (struct service[]){
		(struct service){1095, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1095, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(783, "cnrprotocol", (struct service[]){
		(struct service){1096, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1096, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(784, "sunclustermgr", (struct service[]){
		(struct service){1097, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1097, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(785, "rmiactivation", (struct service[]){
		(struct service){1098, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1098, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(786, "rmiregistry", (struct service[]){
		(struct service){1099, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1099, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(787, "mctp", (struct service[]){
		(struct service){1100, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1100, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(788, "pt2-discover", (struct service[]){
		(struct service){1101, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1101, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(789, "adobeserver-1", (struct service[]){
		(struct service){1102, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1102, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(790, "adobeserver-2", (struct service[]){
		(struct service){1103, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1103, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(791, "xrl", (struct service[]){
		(struct service){1104, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1104, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(792, "ftranhc", (struct service[]){
		(struct service){1105, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1105, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(793, "isoipsigport-1", (struct service[]){
		(struct service){1106, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1106, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(794, "isoipsigport-2", (struct service[]){
		(struct service){1107, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1107, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(795, "ratio-adp", (struct service[]){
		(struct service){1108, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1108, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(796, "webadmstart", (struct service[]){
		(struct service){1110, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(797, "nfsd-keepalive", (struct service[]){
		(struct service){1110, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(798, "lmsocialserver", (struct service[]){
		(struct service){1111, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1111, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(799, "icp", (struct service[]){
		(struct service){1112, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1112, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(800, "ltp-deepspace", (struct service[]){
		(struct service){1113, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1113, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(801, "mini-sql", (struct service[]){
		(struct service){1114, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1114, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(802, "ardus-trns", (struct service[]){
		(struct service){1115, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1115, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(803, "ardus-cntl", (struct service[]){
		(struct service){1116, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1116, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(804, "ardus-mtrns", (struct service[]){
		(struct service){1117, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1117, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(805, "sacred", (struct service[]){
		(struct service){1118, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1118, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(806, "bnetgame", (struct service[]){
		(struct service){1119, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1119, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(807, "bnetfile", (struct service[]){
		(struct service){1120, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1120, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(808, "rmpp", (struct service[]){
		(struct service){1121, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1121, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(809, "availant-mgr", (struct service[]){
		(struct service){1122, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1122, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(810, "murray", (struct service[]){
		(struct service){1123, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1123, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(811, "hpvmmcontrol", (struct service[]){
		(struct service){1124, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1124, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(812, "hpvmmagent", (struct service[]){
		(struct service){1125, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1125, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(813, "hpvmmdata", (struct service[]){
		(struct service){1126, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1126, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(814, "kwdb-commn", (struct service[]){
		(struct service){1127, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1127, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(815, "saphostctrl", (struct service[]){
		(struct service){1128, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1128, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(816, "saphostctrls", (struct service[]){
		(struct service){1129, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1129, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(817, "casp", (struct service[]){
		(struct service){1130, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1130, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(818, "caspssl", (struct service[]){
		(struct service){1131, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1131, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(819, "kvm-via-ip", (struct service[]){
		(struct service){1132, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1132, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(820, "dfn", (struct service[]){
		(struct service){1133, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1133, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(821, "aplx", (struct service[]){
		(struct service){1134, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1134, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(822, "omnivision", (struct service[]){
		(struct service){1135, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1135, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(823, "hhb-gateway", (struct service[]){
		(struct service){1136, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1136, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(824, "trim", (struct service[]){
		(struct service){1137, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1137, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(825, "encrypted-admin", (struct service[]){
		(struct service){1138, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1138, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(826, "evm", (struct service[]){
		(struct service){1139, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1139, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(827, "autonoc", (struct service[]){
		(struct service){1140, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1140, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(828, "mxomss", (struct service[]){
		(struct service){1141, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1141, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(829, "edtools", (struct service[]){
		(struct service){1142, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1142, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(830, "imyx", (struct service[]){
		(struct service){1143, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1143, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(831, "fuscript", (struct service[]){
		(struct service){1144, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1144, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(832, "x9-icue", (struct service[]){
		(struct service){1145, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1145, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(833, "audit-transfer", (struct service[]){
		(struct service){1146, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1146, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(834, "capioverlan", (struct service[]){
		(struct service){1147, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1147, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(835, "elfiq-repl", (struct service[]){
		(struct service){1148, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1148, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(836, "bvtsonar", (struct service[]){
		(struct service){1149, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1149, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(837, "blaze", (struct service[]){
		(struct service){1150, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1150, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(838, "unizensus", (struct service[]){
		(struct service){1151, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1151, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(839, "winpoplanmess", (struct service[]){
		(struct service){1152, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1152, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(840, "c1222-acse", (struct service[]){
		(struct service){1153, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1153, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(841, "resacommunity", (struct service[]){
		(struct service){1154, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1154, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(842, "nfa", (struct service[]){
		(struct service){1155, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1155, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(843, "iascontrol-oms", (struct service[]){
		(struct service){1156, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1156, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(844, "iascontrol", (struct service[]){
		(struct service){1157, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1157, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(845, "dbcontrol-oms", (struct service[]){
		(struct service){1158, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1158, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(846, "oracle-oms", (struct service[]){
		(struct service){1159, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1159, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(847, "olsv", (struct service[]){
		(struct service){1160, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1160, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(848, "health-polling", (struct service[]){
		(struct service){1161, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1161, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(849, "health-trap", (struct service[]){
		(struct service){1162, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1162, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(850, "sddp", (struct service[]){
		(struct service){1163, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1163, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(851, "qsm-proxy", (struct service[]){
		(struct service){1164, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1164, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(852, "qsm-gui", (struct service[]){
		(struct service){1165, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1165, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(853, "qsm-remote", (struct service[]){
		(struct service){1166, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1166, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(854, "cisco-ipsla", (struct service[]){
		(struct service){1167, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1167, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(855, "vchat", (struct service[]){
		(struct service){1168, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1168, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(856, "tripwire", (struct service[]){
		(struct service){1169, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1169, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(857, "atc-lm", (struct service[]){
		(struct service){1170, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1170, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(858, "atc-appserver", (struct service[]){
		(struct service){1171, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1171, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(859, "dnap", (struct service[]){
		(struct service){1172, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1172, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(860, "d-cinema-rrp", (struct service[]){
		(struct service){1173, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1173, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(861, "fnet-remote-ui", (struct service[]){
		(struct service){1174, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1174, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(862, "dossier", (struct service[]){
		(struct service){1175, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1175, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(863, "indigo-server", (struct service[]){
		(struct service){1176, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1176, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(864, "dkmessenger", (struct service[]){
		(struct service){1177, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1177, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(865, "sgi-storman", (struct service[]){
		(struct service){1178, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1178, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(866, "b2n", (struct service[]){
		(struct service){1179, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1179, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(867, "mc-client", (struct service[]){
		(struct service){1180, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1180, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(868, "3comnetman", (struct service[]){
		(struct service){1181, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1181, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(869, "accelenet", (struct service[]){
		(struct service){1182, IPPROTO_TCP, SOCK_STREAM},
	});
	add_entry(870, "accelenet-data", (struct service[]){
		(struct service){1182, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(871, "llsurfup-http", (struct service[]){
		(struct service){1183, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1183, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(872, "llsurfup-https", (struct service[]){
		(struct service){1184, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1184, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(873, "catchpole", (struct service[]){
		(struct service){1185, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1185, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(874, "mysql-cluster", (struct service[]){
		(struct service){1186, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1186, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(875, "alias", (struct service[]){
		(struct service){1187, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1187, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(876, "hp-webadmin", (struct service[]){
		(struct service){1188, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1188, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(877, "unet", (struct service[]){
		(struct service){1189, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1189, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(878, "commlinx-avl", (struct service[]){
		(struct service){1190, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1190, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(879, "gpfs", (struct service[]){
		(struct service){1191, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1191, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(880, "caids-sensor", (struct service[]){
		(struct service){1192, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1192, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(881, "fiveacross", (struct service[]){
		(struct service){1193, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1193, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(882, "openvpn", (struct service[]){
		(struct service){1194, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1194, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(883, "rsf-1", (struct service[]){
		(struct service){1195, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1195, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(884, "netmagic", (struct service[]){
		(struct service){1196, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1196, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(885, "carrius-rshell", (struct service[]){
		(struct service){1197, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1197, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(886, "cajo-discovery", (struct service[]){
		(struct service){1198, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1198, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(887, "dmidi", (struct service[]){
		(struct service){1199, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1199, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(888, "scol", (struct service[]){
		(struct service){1200, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1200, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(889, "nucleus-sand", (struct service[]){
		(struct service){1201, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1201, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(890, "caiccipc", (struct service[]){
		(struct service){1202, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1202, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(891, "ssslic-mgr", (struct service[]){
		(struct service){1203, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1203, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(892, "ssslog-mgr", (struct service[]){
		(struct service){1204, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1204, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(893, "accord-mgc", (struct service[]){
		(struct service){1205, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1205, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(894, "anthony-data", (struct service[]){
		(struct service){1206, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1206, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(895, "metasage", (struct service[]){
		(struct service){1207, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1207, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(896, "seagull-ais", (struct service[]){
		(struct service){1208, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1208, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(897, "ipcd3", (struct service[]){
		(struct service){1209, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1209, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(898, "eoss", (struct service[]){
		(struct service){1210, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1210, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(899, "groove-dpp", (struct service[]){
		(struct service){1211, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1211, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(900, "lupa", (struct service[]){
		(struct service){1212, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1212, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(901, "mpc-lifenet", (struct service[]){
		(struct service){1213, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1213, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(902, "kazaa", (struct service[]){
		(struct service){1214, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1214, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(903, "scanstat-1", (struct service[]){
		(struct service){1215, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1215, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(904, "etebac5", (struct service[]){
		(struct service){1216, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1216, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(905, "hpss-ndapi", (struct service[]){
		(struct service){1217, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1217, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(906, "aeroflight-ads", (struct service[]){
		(struct service){1218, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1218, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(907, "aeroflight-ret", (struct service[]){
		(struct service){1219, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1219, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(908, "qt-serveradmin", (struct service[]){
		(struct service){1220, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1220, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(909, "sweetware-apps", (struct service[]){
		(struct service){1221, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1221, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(910, "nerv", (struct service[]){
		(struct service){1222, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1222, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(911, "tgp", (struct service[]){
		(struct service){1223, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1223, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(912, "vpnz", (struct service[]){
		(struct service){1224, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1224, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(913, "slinkysearch", (struct service[]){
		(struct service){1225, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1225, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(914, "stgxfws", (struct service[]){
		(struct service){1226, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1226, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(915, "dns2go", (struct service[]){
		(struct service){1227, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1227, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(916, "florence", (struct service[]){
		(struct service){1228, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1228, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(917, "zented", (struct service[]){
		(struct service){1229, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1229, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(918, "periscope", (struct service[]){
		(struct service){1230, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1230, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(919, "menandmice-lpm", (struct service[]){
		(struct service){1231, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1231, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(920, "first-defense", (struct service[]){
		(struct service){1232, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1232, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(921, "univ-appserver", (struct service[]){
		(struct service){1233, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1233, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(922, "search-agent", (struct service[]){
		(struct service){1234, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1234, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(923, "mosaicsyssvc1", (struct service[]){
		(struct service){1235, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1235, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(924, "bvcontrol", (struct service[]){
		(struct service){1236, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1236, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(925, "tsdos390", (struct service[]){
		(struct service){1237, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1237, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(926, "hacl-qs", (struct service[]){
		(struct service){1238, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1238, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(927, "nmsd", (struct service[]){
		(struct service){1239, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1239, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(928, "instantia", (struct service[]){
		(struct service){1240, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1240, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(929, "nessus", (struct service[]){
		(struct service){1241, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1241, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(930, "nmasoverip", (struct service[]){
		(struct service){1242, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1242, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(931, "serialgateway", (struct service[]){
		(struct service){1243, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1243, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(932, "isbconference1", (struct service[]){
		(struct service){1244, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1244, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(933, "isbconference2", (struct service[]){
		(struct service){1245, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1245, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(934, "payrouter", (struct service[]){
		(struct service){1246, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1246, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(935, "visionpyramid", (struct service[]){
		(struct service){1247, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1247, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(936, "hermes", (struct service[]){
		(struct service){1248, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1248, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(937, "mesavistaco", (struct service[]){
		(struct service){1249, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1249, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(938, "swldy-sias", (struct service[]){
		(struct service){1250, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1250, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(939, "servergraph", (struct service[]){
		(struct service){1251, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1251, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(940, "bspne-pcc", (struct service[]){
		(struct service){1252, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1252, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(941, "q55-pcc", (struct service[]){
		(struct service){1253, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1253, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(942, "de-noc", (struct service[]){
		(struct service){1254, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1254, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(943, "de-cache-query", (struct service[]){
		(struct service){1255, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1255, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(944, "de-server", (struct service[]){
		(struct service){1256, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1256, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(945, "shockwave2", (struct service[]){
		(struct service){1257, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1257, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(946, "opennl", (struct service[]){
		(struct service){1258, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1258, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(947, "opennl-voice", (struct service[]){
		(struct service){1259, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1259, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(948, "ibm-ssd", (struct service[]){
		(struct service){1260, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1260, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(949, "mpshrsv", (struct service[]){
		(struct service){1261, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1261, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(950, "qnts-orb", (struct service[]){
		(struct service){1262, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1262, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(951, "dka", (struct service[]){
		(struct service){1263, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1263, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(952, "prat", (struct service[]){
		(struct service){1264, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1264, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(953, "dssiapi", (struct service[]){
		(struct service){1265, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1265, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(954, "dellpwrappks", (struct service[]){
		(struct service){1266, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1266, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(955, "epc", (struct service[]){
		(struct service){1267, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1267, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(956, "propel-msgsys", (struct service[]){
		(struct service){1268, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1268, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(957, "watilapp", (struct service[]){
		(struct service){1269, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1269, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(958, "opsmgr", (struct service[]){
		(struct service){1270, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1270, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(959, "excw", (struct service[]){
		(struct service){1271, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1271, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(960, "cspmlockmgr", (struct service[]){
		(struct service){1272, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1272, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(961, "emc-gateway", (struct service[]){
		(struct service){1273, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1273, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(962, "t1distproc", (struct service[]){
		(struct service){1274, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1274, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(963, "ivcollector", (struct service[]){
		(struct service){1275, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1275, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(964, "miva-mqs", (struct service[]){
		(struct service){1277, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1277, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(965, "dellwebadmin-1", (struct service[]){
		(struct service){1278, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1278, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(966, "dellwebadmin-2", (struct service[]){
		(struct service){1279, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1279, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(967, "pictrography", (struct service[]){
		(struct service){1280, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1280, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(968, "healthd", (struct service[]){
		(struct service){1281, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1281, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(969, "emperion", (struct service[]){
		(struct service){1282, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1282, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(970, "productinfo", (struct service[]){
		(struct service){1283, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1283, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(971, "iee-qfx", (struct service[]){
		(struct service){1284, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1284, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(972, "neoiface", (struct service[]){
		(struct service){1285, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1285, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(973, "netuitive", (struct service[]){
		(struct service){1286, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1286, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(974, "routematch", (struct service[]){
		(struct service){1287, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1287, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(975, "navbuddy", (struct service[]){
		(struct service){1288, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1288, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(976, "jwalkserver", (struct service[]){
		(struct service){1289, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1289, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(977, "winjaserver", (struct service[]){
		(struct service){1290, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1290, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(978, "seagulllms", (struct service[]){
		(struct service){1291, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1291, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(979, "dsdn", (struct service[]){
		(struct service){1292, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1292, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(980, "pkt-krb-ipsec", (struct service[]){
		(struct service){1293, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1293, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(981, "cmmdriver", (struct service[]){
		(struct service){1294, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1294, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(982, "ehtp", (struct service[]){
		(struct service){1295, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1295, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(983, "dproxy", (struct service[]){
		(struct service){1296, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1296, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(984, "sdproxy", (struct service[]){
		(struct service){1297, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1297, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(985, "lpcp", (struct service[]){
		(struct service){1298, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1298, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(986, "hp-sci", (struct service[]){
		(struct service){1299, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1299, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(987, "h323hostcallsc", (struct service[]){
		(struct service){1300, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1300, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(988, "sftsrv", (struct service[]){
		(struct service){1303, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1303, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(989, "boomerang", (struct service[]){
		(struct service){1304, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1304, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(990, "pe-mike", (struct service[]){
		(struct service){1305, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1305, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(991, "re-conn-proto", (struct service[]){
		(struct service){1306, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1306, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(992, "pacmand", (struct service[]){
		(struct service){1307, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1307, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(993, "odsi", (struct service[]){
		(struct service){1308, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1308, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(994, "jtag-server", (struct service[]){
		(struct service){1309, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1309, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(995, "husky", (struct service[]){
		(struct service){1310, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1310, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(996, "rxmon", (struct service[]){
		(struct service){1311, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1311, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(997, "sti-envision", (struct service[]){
		(struct service){1312, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1312, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(998, "bmc-patroldb", (struct service[]){
		(struct service){1313, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1313, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(999, "pdps", (struct service[]){
		(struct service){1314, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1314, IPPROTO_UDP, SOCK_DGRAM},
	});
	add_entry(1000, "els", (struct service[]){
		(struct service){1315, IPPROTO_TCP, SOCK_STREAM},
		(struct service){1315, IPPROTO_UDP, SOCK_DGRAM},
	});
}
