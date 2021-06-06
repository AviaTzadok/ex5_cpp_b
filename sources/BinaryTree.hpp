#pragma once
#include <iostream>
#include <stack>
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>

namespace ariel{
    template<typename T>
    class BinaryTree
    {
private:
    class Node
    {
    public:
        T value;
        Node* left;
        Node* right;
        Node(T val) : value(val){
            left = nullptr;
            right = nullptr;}
        Node(const Node& node):value(node.value)
        {
            if(node.left != nullptr){
                left = new Node(*node.left);}
            else{left = nullptr;}
            if(node.right != nullptr){
                right = new Node(*node.right);}
            else{right = nullptr;}                        
        }


        Node(const Node&& node) noexcept : value(node.value) 
        {
            left = node.left;
            right = node.right;
            node.left = nullptr;
            node.right = nullptr;
        }
        Node& operator=(const Node& other)
        {
            if(this == &other){return *this;}
            value = other.value;
            left = new Node(*other.left);
            right = new Node(*other.right);
            return *this;
        }
        Node& operator=(Node&& other) noexcept
        {
            this->root = other.root;
            other.root = nullptr;
            return *this;
        }
        bool hasLeft(){return left != nullptr;}
        bool hasRight(){return right != nullptr;}
        ~Node(){}
};

        Node* head;
        void freetree(Node* node)
        {
            if(node == nullptr){return;}
            freetree(node->right);
            node->right = nullptr;
            freetree(node->left);
            node->left = nullptr;
            delete node;
        }

    public:
        BinaryTree(){head = nullptr;}
        BinaryTree(const BinaryTree& other)
        {
            if(other.head == nullptr){head = nullptr;
            return;}
            head = new Node(*other.head);
        }
        BinaryTree(BinaryTree&& other) noexcept
        {
            head = other.head;
            other.head = nullptr;
        }
        ~BinaryTree(){freetree(head);}

        BinaryTree& add_root(T value);
        BinaryTree& add_left(T existing_node, T new_node);
        BinaryTree& add_right(T existing_node, T new_node);

        BinaryTree& operator=(const BinaryTree& other)
        {
            if(this == &other){return *this;}
            if(head == nullptr){head = new Node(*other.head);}
            else{
                if(head != nullptr){freetree(head);}
                head = new Node(*other.head);
            }
            return *this;
        }

        BinaryTree& operator=(BinaryTree&& other) noexcept
        {
            if(head != nullptr){freetree(head);}
            head = other.head;
            other.head = nullptr;
            return *this;
        }

        void print()
        {
            print_all(head);
        }
        void print_all(Node* n)
        {
            if(n == nullptr){return;}
            std::cout << n->value;
            print_all(n->right);
            print_all(n->left);
        }

        Node* find(Node* current, T val)
        {
            if(current == nullptr){
                return nullptr;
            }
            if(current->value == val){
                return current;
            }
            Node* left = find(current->left, val);
            if(left != nullptr){
                return left;
            }
            return find(current->right, val);
        }



        class preorder_iterator{
            private:
                std::stack<Node*> st;
                void next()
                {
                    if(st.empty()){return;}
                    if(st.top()->hasLeft()){st.push(st.top()->left);}
                    else{
                        while (!st.empty() && !st.top()->hasRight()){st.pop();}
                        if(!st.empty()){
                            Node* n = st.top();
                            st.pop();
                            st.push(n->right);
                        }
                    }
                }
            public:    
                preorder_iterator(Node* ptr = nullptr)
                {
                    if(ptr != nullptr){st.push(ptr);}
                }
                preorder_iterator& operator++()
                {
                    next();
                    return *this;
                }
                preorder_iterator operator++(int)
                {
                    preorder_iterator tmp = *this;
                    next();
                    return tmp;
                }


                
                bool operator==(const preorder_iterator& it)
                {
                    if(!st.empty() && !it.st.empty()){return st.top() == it.st.top();}
                    return st.empty() && it.st.empty();
                }
                bool operator!=(const preorder_iterator& it)
                {
                    if(!st.empty() && !it.st.empty()){return st.top() != it.st.top();}
                    return !(st.empty() && it.st.empty());
                }
                T& operator*(){return st.top()->value;}
                T* operator->(){return &st.top()->value;}


        };

        class inorder_iterator{
            private:
                std::stack<Node*> st;
                void get_path()
                {
                    while (st.top()->hasLeft()){
                        st.push(st.top()->left);
                    }
                }
                void next()
                {
                    if(st.empty()){return;}
                    if(!st.top()->hasRight()){
                        st.pop();
                    }
                    else{
                        Node* n = st.top();
                        st.pop();
                        st.push(n->right);
                        get_path();
                    }
                }
            public:    
                inorder_iterator(Node* ptr = nullptr)
                {
                    if(ptr != nullptr){
                        st.push(ptr);
                        get_path();
                    }
                }
                inorder_iterator& operator++()
                {
                    next();
                    return *this;
                }
                inorder_iterator operator++(int)
                {
                    inorder_iterator tmp = *this;
                    next();
                    return tmp;
                }
                bool operator==(const inorder_iterator& it)
                {
                    if(!st.empty() && !it.st.empty()){return st.top() == it.st.top();}
                    return st.empty() && it.st.empty();
                }
                bool operator!=(const inorder_iterator& it)
                {
                    if(!st.empty() && !it.st.empty()){return st.top() != it.st.top();}
                    return !(st.empty() && it.st.empty());
                }
                T& operator*(){return st.top()->value;}
                T* operator->(){return &st.top()->value;}

                
        };

        class postorder_iterator{
            private:
                std::stack<Node*> st;
                std::stack<Node*> visited;

                void get_path()
                {
                    while(st.top()->hasLeft() || st.top()->hasRight())
                    {
                        if(st.top()->hasLeft()){st.push(st.top()->left);}
                        else if(st.top()->hasRight()){
                            visited.push(st.top());
                            st.push(st.top()->right);
                        }
                    }
                }
                void next()
                {
                    if(st.empty()){return;}
                    if(!visited.empty() && st.top() == visited.top()){visited.pop();}
                    st.pop();
                    if(st.empty() || (!visited.empty() && st.top() == visited.top())){return;}
                    if(st.top()->hasRight()){
                        visited.push(st.top());
                        st.push(st.top()->right);
                        get_path();
                    }
                }
            public:
                postorder_iterator(Node* ptr = nullptr)
                {
                    if(ptr != nullptr){
                        st.push(ptr);
                        get_path();
                    }
                }
                postorder_iterator& operator++()
                {
                    next();
                    return *this;
                }
                postorder_iterator operator++(int)
                {
                    postorder_iterator tmp = *this;
                    next();
                    return tmp;
                }
                bool operator==(const postorder_iterator& it)
                {
                    if(!st.empty() && !it.st.empty()){return st.top() == it.st.top();}
                    return st.empty() && it.st.empty();
                }
                bool operator!=(const postorder_iterator& it)
                {
                    if(!st.empty() && !it.st.empty()){return st.top() != it.st.top();}
                    return !(st.empty() && it.st.empty());
                }
                T& operator*(){return st.top()->value;}
                T* operator->(){return &st.top()->value;}
        };

        inorder_iterator begin(){return inorder_iterator(head);}
        inorder_iterator end(){return inorder_iterator();}
        preorder_iterator begin_preorder(){return preorder_iterator(head);}
        preorder_iterator end_preorder(){ return preorder_iterator();}
        inorder_iterator begin_inorder(){return inorder_iterator(head);}
        inorder_iterator end_inorder(){return inorder_iterator();}
        postorder_iterator begin_postorder(){return postorder_iterator(head);}
        postorder_iterator end_postorder(){return postorder_iterator();}
        std::ostream& str(std::ostream& c){return c;}
        friend std::ostream& operator<<(std::ostream& output,const BinaryTree<T> &tree){return output;}


};

    template <typename T>
    BinaryTree<T>& BinaryTree<T>::add_root(T value)
    {
        if(head == nullptr){this->head = new Node(value);
        }
        else{head->value = value;
        }
        return *this;
    }
    template <typename T>
    BinaryTree<T>& BinaryTree<T>::add_left(T existing_node, T new_node)
    {
        Node* n = find(this->head, existing_node);
        if(n == nullptr){throw "not exist";
        }
        if(n->left == nullptr){n->left = new Node(new_node);
        }
        else{(n->left)->value = new_node;
        }
        return *this;
        
    }
    template <typename T>

    
    BinaryTree<T>& BinaryTree<T>::add_right(T existing_node, T new_node)
    {
        Node* n = find(this->head, existing_node);
        if(n == nullptr){throw "not exist";
        }
        if(n->right == nullptr){n->right = new Node(new_node);
        }
        else{(n->right)->value = new_node;
        }
        return *this;
    }
}



















































































