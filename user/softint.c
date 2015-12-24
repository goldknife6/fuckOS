// buggy program - causes an illegal software interrupt



int main(int argc, char **argv)
{
	asm volatile("int $14");	// page fault
	return 0;
}

