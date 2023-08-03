void initialize()
{
	int i, j;
	fprintf(intermediate, "INITIALIZE:\n");
	fprintf(intermediate, "PUSH BP\n");
	fprintf(intermediate, "MOV BP, SP\n");
	fprintf(intermediate, "MOV R0,0\n");
	fprintf(intermediate, "MOV R1,1\n");
	fprintf(intermediate, "L0:\n");
	fprintf(intermediate, "MOV [R0],-1\n");
	fprintf(intermediate, "ADD R0,R1\n");
	fprintf(intermediate, "MOV R2,255\n");
	fprintf(intermediate, "GE R2,R0\n");
	fprintf(intermediate, "JZ R2,L1\n");
	fprintf(intermediate, "JMP L0\n");
	fprintf(intermediate, "L1:\n"); /*	j = 0;
												while(j <= 255) do	
													j = j + 1;
												endwhile;
											*/
	fprintf(intermediate, "MOV R0,0\n");
	fprintf(intermediate, "MOV R1,16\n");
	fprintf(intermediate, "MOV R3,16\n");
	fprintf(intermediate, "L2:\n");
	fprintf(intermediate, "MOV [R0],R1\n");
	fprintf(intermediate, "ADD R1,R3\n");
	fprintf(intermediate, "ADD R0,R3\n");
	fprintf(intermediate, "MOV R2,255\n");
	fprintf(intermediate, "GE R2,R0\n");
	fprintf(intermediate, "JZ R2,L3\n");
	fprintf(intermediate, "JMP L2\n");
	fprintf(intermediate, "L3:\n"); /*i = 0; j = 16;
											while(j <= 255)
											{
												fprintf(intermediate, "MOV [%d], %d\n",i,j);
												i = j;
												j = j + 16;
											}
											fprintf(intermediate, "MOV [%d], %d\n",256,0);*/
	fprintf(intermediate, "MOV [240],-1\n");
	fprintf(intermediate, "MOV [256],0\n");
	fprintf(intermediate, "MOV BP, [SP]\n");
	fprintf(intermediate, "POP R0\n");
	fprintf(intermediate, "RET\n");
	return;
}
void alloc() // put the value in BP - 2
{
	int r1, r2, r3;

	fprintf(intermediate, "ALLOC:\n");
	fprintf(intermediate, "PUSH BP\n");
	fprintf(intermediate, "MOV BP, SP\n");
	fprintf(intermediate, "MOV R%d, [256]\n", 0);
	fprintf(intermediate, "MOV R%d, [R%d]\n", 1, 0);
	fprintf(intermediate, "MOV [%d], R%d\n", 256, 1);
	fprintf(intermediate, "MOV R%d, BP\n", 1);
	fprintf(intermediate, "MOV R%d, 2\n", 2);
	fprintf(intermediate, "SUB R%d, R%d\n", 1, 2);
	fprintf(intermediate, "MOV [R%d], R%d\n", 1, 0);
	fprintf(intermediate, "MOV BP, [SP]\n");
	fprintf(intermediate, "POP R0\n");
	fprintf(intermediate, "RET\n");
	return;
}
void freeheap() // get the argument from BP -3
{
	int r1, r2;
	fprintf(intermediate, "FREE:\n");
	fprintf(intermediate, "PUSH BP\n");
	fprintf(intermediate, "MOV BP, SP\n");
	fprintf(intermediate, "MOV R%d, 2\n", 0);
	fprintf(intermediate, "MOV R%d, BP\n", 1);
	fprintf(intermediate, "SUB R%d, R%d\n", 1, 0);
	fprintf(intermediate, "MOV R%d, [R%d]\n", 0, 1);
	fprintf(intermediate, "MOV R%d, [256]\n", 1);
	fprintf(intermediate, "MOV [%d], R%d\n", 256, 0);
	fprintf(intermediate, "MOV [R%d], R%d\n", 0, 1);
	fprintf(intermediate, "MOV BP, [SP]\n");
	fprintf(intermediate, "POP R0\n");
	fprintf(intermediate, "RET\n");
	return;
}