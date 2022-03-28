#include "minishell.h"

void printest(struct s_tnode *tnode)

{
	
	while(tnode)
	{
		printf("TEST %s\n", tnode ->str);
		tnode->type = STRING;
		tnode = tnode->next;
	}
}
void	test(struct s_tnode *tnode)
{
		while(tnode)
		{
		
		
			if(tnode->str[0] == '|')
				tnode->type = PIPE;
			else if(tnode->str[0] == '>')
				tnode->type = REDIR_R;
			else if(tnode->str[0] == '<')
				tnode->type = REDIR_L;
			else if(tnode->str[0] == '>' && tnode->str[1] == '>')
				tnode->type = APPEND;
		
		tnode = tnode->next;
		}
}

void	tester(struct s_tnode *tnode)
{
	while(tnode)
	{
		if(tnode->type == PIPE)
			printf("PIPE OK\n");
		if(tnode->type == APPEND)
			printf("APPEND OK\n");
		if(tnode->type == REDIR_L)
			printf("REDIR_L OK\n");
		if(tnode->type == REDIR_R)
			printf("REDIR_R OK\n");
		if(tnode->type == STRING)
			printf("STRING OK\n");
		tnode = tnode->next;
	}
}
