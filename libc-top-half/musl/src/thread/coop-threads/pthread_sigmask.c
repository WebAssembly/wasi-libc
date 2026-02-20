struct sigset_t;
int pthread_sigmask(int how, const sigset_t *restrict set, sigset_t *restrict old)
{
	return ENOTSUP;
}
