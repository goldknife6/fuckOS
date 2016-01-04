#ifndef _MINIOS_LIBFS_H
#define _MINIOS_LIBFS_H
#include <fuckOS/fs.h>

extern struct file_operations simple_dir_operations;
extern struct file_operations simple_file_operations;


extern struct inode_operations simple_dir_inode_operations;
extern struct inode_operations simple_file_inode_operations;

extern struct super_operations simple_super_operations;

extern int simple_dir_read(struct file * ,char* , int , int);
extern int simple_file_read(struct file * ,char* , int , int);

extern struct dentry * simple_dir_inode_lookup(struct inode *,struct dentry *, struct nameidata* );
extern int simple_dir_inode_create(struct inode *,struct dentry *, int ,struct nameidata* );
extern int simple_dir_inode_mkdir(struct inode *,struct dentry *,int );
extern int simple_dir_inode_rmdir(struct inode *,struct dentry *);
extern int simple_dir_inode_mknod(struct inode *,struct dentry *,int ,int );
extern int simple_dir_inode_rename(struct inode *, struct dentry *,struct inode *, struct dentry *);


extern struct dentry *simple_file_inode_lookup(struct inode *,struct dentry *, struct nameidata* );
extern int simple_file_inode_create(struct inode *,struct dentry *, int ,struct nameidata* );
extern int simple_file_inode_mkdir(struct inode *,struct dentry *,int );
extern int simple_file_inode_rmdir(struct inode *,struct dentry *);
extern int simple_file_inode_mknod(struct inode *,struct dentry *,int ,int );
extern int simple_file_inode_rename(struct inode *, struct dentry *,struct inode *, struct dentry *);


extern struct inode *simple_super_alloc_inode(struct super_block *);
extern void simple_super_destroy_inode(struct inode *);
extern int simple_super_read_inode(struct inode *);
extern int simple_super_read_super(struct super_block * , void *, int );

extern struct inode *simple_get_inode(struct super_block *,int, int);
extern struct inode *create_node(struct dentry *, struct qstr *,int ,struct file_operations *,struct inode_operations *);
#endif/*!_MINIOS_LIBFS_H*/
