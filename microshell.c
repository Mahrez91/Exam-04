#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct s_list{
    int current_cmd;
    int pipe;
    char **multiple_cmd;
    char *uniq_cmd;
    struct s_list *next;

}               t_list;

int ft_strlen(char const *str)
{
	int	i;

	i = 0;
	while (str && str[i])
		i++;
	return (i);
}


char *ft_strdup_ns(char const *str)
{
	char	*copy;
	int		i;

	if (!(copy = (char*)malloc(sizeof(*copy) * (ft_strlen(str) + 2))))
		return (NULL);
	i = 0;
	while (str[i])
	{
		copy[i] = str[i];
		i++;
	}
    copy[i] = '\0';
	return (copy);
}

char *ft_strdup(char const *str)
{
	char	*copy;
	int		i;

	if (!(copy = (char*)malloc(sizeof(*copy) * (ft_strlen(str) + 2))))
		return (NULL);
	i = 0;
	while (str[i])
	{
		copy[i] = str[i];
		i++;
	}
    copy[i] = 32;
    copy[i + 1] = '\0';
	return (copy);
}

char *ft_strcat(char *s1, char *s2)
{
    int i = 0;
    int j = 0;
    char *new;

    new = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 2));
    while(s1[i])
    {
        new[i] = s1[i];
        i++;
    }
    while(s2[j])
    {
        new[i] = s2[j];
        i++;
        j++;
    }
    new[i] = 32;
    new[i + 1] = '\0';
    free(s1);
    return (new);
}

char **ft_split_cmd(char **argv)
{
    char **cmd;
    int count = 0;
    int the_cmd = 0;
    int i = 1;

    while (argv[i] && (strncmp(argv[i], ";", 1) == 0 && ft_strlen(argv[i]) == 1))
        i++;
    if (!argv[i])
    {
        exit(0);
    }    
    cmd = malloc(sizeof(char *) * 1000);
    cmd[count] = ft_strdup(argv[i]);
    i++;
    while (argv[i])
    {
        if ((strncmp(argv[i], ";", 1) == 0 && ft_strlen(argv[i]) == 1))
        {
            count++;
            cmd[count] = ft_strdup_ns("");
            i++;
        }
        else
        {
            cmd[count] = ft_strcat(cmd[count], argv[i]);
            i++;
        }
    }
    cmd[count + 1] = NULL;
    return cmd;
}

void    free_str(char **str)
{
    int line = 0;
    while (str[line])
    {
        free(str[line]);
        line++;
    }
    free(str);
}

char **ft_split(char *str, char c)
{
    char **cmd;
    int count = 0;
    int the_cmd = 0;
    int i = 0;

    cmd = malloc(sizeof(char *) * 1000);
    cmd[count] = malloc(sizeof(char) * 2000);
    while (str[i])
    {
        if (str[i] == c)
        {
            cmd[count][the_cmd] = '\0';
            count++;
            the_cmd = 0;
            cmd[count] = malloc(sizeof(char) * 2000);
            i++;
            while (str[i] == 32)
                i++;
        }
        else
        {
            cmd[count][the_cmd] = str[i];
            the_cmd++;
            i++;
        }
    }
    if (the_cmd == 0)
    {    
        free(cmd[count]);
        cmd[count] = NULL;
    }
    else
    {
        cmd[count][the_cmd] = '\0';
        cmd[count + 1] = NULL;
    }
    return cmd;
}

int ft_strchr(char *str, char c)
{
    int i = 0;
    int count = 0;

    while (str[i])
    {
        if (str[i] == c)
            count++;
        i++;
    }
    return count;
}

t_list  *ft_lstnew(char *cmd, int nb)
{
    t_list *lst;

    lst = malloc(sizeof(t_list));
    if (!lst)
        exit(1);
    lst->current_cmd = nb;
    lst->pipe = ft_strchr(cmd, '|');
    if (lst->pipe == 0)
    {    
        lst->uniq_cmd = ft_strdup_ns(cmd);
        lst->multiple_cmd = NULL;
    }
    else
    {
        
        lst->multiple_cmd = ft_split(cmd, '|');
        lst->uniq_cmd = NULL;
    }
    lst->next = NULL;
    return lst;
}

void ft_lstadd_back(t_list **alst, t_list *new)
{
    t_list *lst;

    lst = *alst;
    if (alst == NULL)
        *alst = new;
    else
    {
        while (lst->next)
            lst = lst->next;
        lst->next = new;
    }
}

void    free_list(t_list *lst)
{
    int i = 0;
    t_list *element;

    while (lst->next)
    {
        element = lst->next;
        if (lst->pipe == 0)
            free(lst->uniq_cmd);
        else
            free_str(lst->multiple_cmd);
        free(lst);
        lst = element;
    }
    lst = element;
    if (lst->pipe == 0)
        free(lst->uniq_cmd);
    else
        free_str(lst->multiple_cmd);
     free(lst);
}

void pipex(char **cmd, char **env)
{
    int pid;
    int status;
    int k;

    pid = fork();
    if (pid != 0)
    {
        waitpid(pid, &status, 0);
		k = WEXITSTATUS(status);
    }
    else
    {    
        k = execve(cmd[0], cmd, env);
        write(2, "error: cannot execute ", ft_strlen("error: cannot execute "));
        write(2, cmd[0], ft_strlen(cmd[0]));
        write(2, "\n", 1);
        exit(0);
    }
}

void	pipex_all(char **cmd, char **env, int i)
{
	int		pipefd[2];
	pid_t	pid1;
	int		status;

	if (pipe(pipefd) == -1)
		exit(EXIT_FAILURE);
	pid1 = fork();
	if (pid1 == -1)
		exit(EXIT_FAILURE);
	if (pid1)
	{
		close(pipefd[1]);
		dup2(pipefd[0], 0);
		if (i == 0)
			dup2(1, 0);
		waitpid(pid1, &status, 0);
        close(pipefd[0]);
	}
	else
	{
		close(pipefd[0]);
	    if (i != 0)
		    dup2(pipefd[1], 1);
        close(pipefd[1]);
		execve(cmd[0], cmd, env);
        write(2, "error: cannot execute ", ft_strlen("error: cannot execute "));
        write(2, cmd[0], ft_strlen(cmd[0]));
        write(2, "\n", 1);
        exit(0);
	}
}

void    uniq_comm(t_list *lst, char **env)
{
    char **cmd;
    int i = 0;

    cmd = ft_split(lst->uniq_cmd, ' ');
    pipex(cmd, env);
    free_str(cmd);
}

void    multiple_comm(t_list *lst, char **env)
{
    char **cmd;
    int i = 0;

    while (lst->multiple_cmd[i])
    {
        cmd = ft_split(lst->multiple_cmd[i], ' ');        
        pipex_all(cmd, env, lst->pipe);
        i++;
        lst->pipe--;
        free_str(cmd);
    }
}

void cd_comm(t_list *lst)
{
    int i = 0;
    char  **cd;
    cd = ft_split(lst->uniq_cmd, ' ');
    while (cd[i])
        i++;
    if (i != 2)
    {
        write(2, "error: cd: bad arguments", ft_strlen("error: cd bad argument"));
        write(2, "\n", 1);
    }
    i = chdir(cd[1]);
    if (i == -1)
    {
        write(2, "error: cd: cannot change directory to ", ft_strlen("error: cd: cannot change directory to "));
        write(2, cd[1], ft_strlen(cd[1]));
        write(2, "\n", 1);
    }
    free_str(cd);
}

void    parsing(t_list *lst, char **env)
{
    int i = 0;
    while (lst)
    {
        if (lst->pipe == 0 && (strncmp(lst->uniq_cmd, "cd ", 3) == 0 ))
            cd_comm(lst);
        else if (lst->pipe == 0)
            uniq_comm(lst, env);
        else if (lst->pipe > 0)
            multiple_comm(lst, env);
        lst = lst->next;
    }
}

void    make_list(char **cmd, char **env)
{
    int i = 0;
    int nb = 1;
    t_list *lst;

    lst = ft_lstnew(cmd[i], nb);
    i++; 
    nb++;
    while(cmd[i])
    {
        if (cmd[i][0] != '\0')
        {
            ft_lstadd_back(&lst, ft_lstnew(cmd[i], nb));
            nb++;
        }
        i++;
    }
    free_str(cmd);
    parsing(lst, env);
    free_list(lst);
}

int main(int argc, char **argv, char **env)
{
    char **cmd;
    
    cmd = ft_split_cmd(argv);
    make_list(cmd, env);
    //fflush(stdout);
	//system("leaks a.out | grep 'Process'");
    return 0;
}