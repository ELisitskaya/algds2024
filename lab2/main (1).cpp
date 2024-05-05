#include <stdio.h>
#include <stdlib.h>
#include <gtest/gtest.h>
using namespace std;
struct avl_node
{
    int key;
    int height;
    struct avl_node* left;
    struct avl_node* right;
    struct avl_node* parent;
};
struct avl_node* new_node(int key)
{
    struct avl_node* new_node = (struct avl_node*)malloc(sizeof(struct avl_node));
    new_node->key = key;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;
    new_node->height = 1;
    return new_node;
}
int avl_height(struct avl_node* node)
{
    if (node) return node->height;
    else return 0;
}
void new_height(struct avl_node* node)
{
    int left_height = avl_height(node->left);
    int right_height = avl_height(node->right);
    if (left_height > right_height) node->height = left_height+1;
    else node->height = right_height+1;
}
void print_avl(struct avl_node* node, int level)
{
    if (node)
    {
        if (node->left) print_avl(node->left, level + 1);
        for (int i = 0; i < level; i++)
        {
            printf("   ");
        }
        printf("%d\n", node->key);
        if (node->right) print_avl(node->right, level + 1);
    }
}
struct avl_node* right_rotate(struct avl_node* node)
{
    struct avl_node* node1 = node->left;
    struct avl_node* parent = node->parent;
    node->left = node1->right;
    if (node1->right) node1->right->parent = node;
    node1->right = node;
    node1->parent = node->parent;
    node->parent = node1;
    new_height(node);
    new_height(node1);
    if (parent) parent->left = node->parent;
    return node->parent;
}

struct avl_node* left_rotate(struct avl_node* node)
{
    struct avl_node* node1 = node->right;
    struct avl_node* parent = node->parent;
    node->right = node1->left;
    if (node1->left) node1->left->parent = node;
    node1->left = node;
    node1->parent = node->parent;
    node->parent = node1;
    new_height(node);
    new_height(node1);
    if (parent) parent->right = node->parent;
    return node->parent;
}
struct avl_node* avl_balance(struct avl_node* node)
{
    new_height(node);
    int balance = avl_height(node->right) - avl_height(node->left);
    if (balance == 2)
    {
        if (avl_height((node->right)->right) - avl_height((node->right)->left) < 0)
        {
            node->right = right_rotate(node->right);
            node->left = NULL;
        }
        return left_rotate(node);
    }
    if (balance == -2)
    {
        if (avl_height((node->left)->right) - avl_height((node->left)->left) > 0)
        {
            node->left = left_rotate(node->left);
            node->right = NULL;
        }
        return right_rotate(node);
    }
    return node;
}
void avl_root(struct avl_node** node)
{
    while ((*node)->parent)
    {
        (*node) = (*node)->parent;
    }
}
void avl_rebalance(struct avl_node** root, struct avl_node* parent, struct avl_node* new_node)
{
    while (parent)
    {
        new_height(parent);
        int balance = avl_height(parent->right) - avl_height(parent->left);
        if (balance == 0) break;
        else if (abs(balance) == 1)
        {
            new_node = parent;
            parent = parent->parent;
        }
        else
        {
            *root = avl_balance(parent);
            avl_root(&(*root));
            break;
        }
    }
}
void avl_insert_in(struct avl_node** node, struct avl_node** insert_node, struct avl_node* root, int key)
{
    struct avl_node* current = root;
    while (current)
    {
        *node = current;
        if (key < current->key) current = current->left;
        else current = current->right;
    }
    if (key < (*node)->key) (*node)->left = *insert_node;
    else (*node)->right = *insert_node;
    (*insert_node)->parent = (*node);
}

struct avl_node* avl_insert(struct avl_node* node, int key)
{
    struct avl_node* n_node = new_node(key);
    if (!node) return n_node;
    struct avl_node* parent = NULL;
    avl_insert_in(&parent, &n_node, node, key);
    avl_rebalance(&node, parent, n_node);
    return node;
}
struct avl_node* avl_remove(struct avl_node* node, int k)
{
    if (!node) return NULL;
    struct avl_node* parent = NULL;
    struct avl_node* current = node;
    while (current && current->key != k)
    {
        parent = current;
        if (k < current->key) current = current->left;
        else current = current->right;
    }
    if (!current) return node;
    struct avl_node* child = (current->left) ? current->left : current->right;
    if (!parent) node = child;
    else if (parent->left == current) parent->left = child;
    else parent->right = child;
    if (child) child->parent = parent;
    free(current);
    return avl_balance(node);
}
TEST(NEWNODE,key_left_right_parent){
	struct avl_node* node = new_node(1);
	EXPECT_EQ(1, node->key);
	EXPECT_EQ(1, node->height);
	EXPECT_EQ(nullptr, node->left);
	EXPECT_EQ(nullptr, node->right);
	EXPECT_EQ(nullptr, node->parent);
}
TEST (AVL_HIGHT, new_node){
	struct avl_node* node=new_node(2);
	EXPECT_EQ(1, avl_height(node));
}
TEST(AVL_HIGHT, empty_node){
	EXPECT_EQ(0, avl_height(NULL));	
}
TEST(AVL_HIGHT, four_nodes){
	struct avl_node* node=new_node(1);
	node=avl_insert(node, 2);
	node=avl_insert(node, 3);
	node=avl_insert(node, 4);
	EXPECT_EQ(3, avl_height(node));	
}
TEST(REMOVE, node){
	struct avl_node* node=new_node(1);
	node=avl_insert(node, 2);
	node=avl_remove(node, 1);
	EXPECT_EQ(2, node->key);
}
TEST(REMOVE, nonexistent){
	struct avl_node* node=new_node(1);
	node=avl_remove(node, 2);
	EXPECT_EQ(1, node->key);
}
TEST(REMOVE, empty){
	EXPECT_EQ(nullptr, avl_remove(NULL, 2));
}
TEST (INSERT,second_bigger ){
	struct avl_node* node=new_node(1);
	node=avl_insert(node, 2);
	EXPECT_EQ(2, (node->right)->key);	
}
TEST(INSERT, second_smaller){
	struct avl_node* node=new_node(2);
	node=avl_insert(node, 1);
	EXPECT_EQ(1, (node->left)->key);
}
TEST(INSERT, third_with_rotate){
	struct avl_node* node=new_node(1);
	node=avl_insert(node, 2);
	node=avl_insert(node, 3);
	EXPECT_EQ(2, (node->key));
	EXPECT_EQ(1, (node->left)->key);
	EXPECT_EQ(3, (node->right)->key);
	
}
TEST(INSERT, third_with_left_rotate){
	struct avl_node* node=new_node(3);
	node=avl_insert(node, 2);
	node=avl_insert(node, 1);
	EXPECT_EQ(2, (node->key));
	EXPECT_EQ(1, (node->left)->key);
	EXPECT_EQ(3, (node->right)->key);
}
TEST(INSERT, algorythm){
	struct avl_node* node=new_node(4);
	node=avl_insert(node, 5);;
	node=avl_insert(node, 2);
	node=avl_insert(node, 1);
	node=avl_insert(node, 3);
	node=avl_insert(node, 6);
	EXPECT_EQ(4, node->key);
	EXPECT_EQ(5,(node->right)->key);
	EXPECT_EQ(6,((node->right)->right)->key);
	EXPECT_EQ(2, (node->left)->key);
	EXPECT_EQ(3, ((node->left)->right)->key);
	EXPECT_EQ(1, ((node->left)->left)->key);
}
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
