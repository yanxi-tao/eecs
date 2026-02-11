// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C262F4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include <cstddef>
#include <deque>
#include <utility>

#include "Eecs281PQ.hpp"

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
    public:
        // Description: Custom constructor that creates a node containing
        //              the given value.
        explicit Node(const TYPE &val)
            : elt { val } {}

        // Description: Allows access to the element at that Node's position.
        //              There are two versions, getElt() and a dereference
        //              operator, use whichever one seems more natural to you.
        // Runtime: O(1) - this has been provided for you.
        const TYPE &getElt() const { return elt; }
        const TYPE &operator*() const { return elt; }

        // The following line allows you to access any private data
        // members of this Node class from within the PairingPQ class.
        // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
        // function).
        friend PairingPQ;

    private:
        TYPE elt;
        Node *child = nullptr;
        Node *sibling = nullptr;
        Node *previous = nullptr;
    };  // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp } {
        // TODO: Implement this function.
    }  // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp } {
        std::deque<Node*> pass;

        while (start != end) {
            Node *a = new Node(*start);
            ++start;
            ++count;
            if (start != end) {
                Node *b = new Node(*start);
                ++start;
                ++count;
                pass.push_back(meld(a, b));
            } else {
                pass.push_back(a);
            }
        }

        root = pass.back();
        pass.pop_back();

        while (!pass.empty()) {
            root = meld(root, pass.back());
            pass.pop_back();
        }
    }  // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other)
        : BaseClass { other.compare } {
        root = copyNode(other.root);
        count = other.count;
    }  // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        PairingPQ temp(rhs);
        std::swap(root, temp.root);
        std::swap(count, temp.count);
        return *this;
    }  // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        destroyNode(root);
    }  // ~PairingPQ()


    // Description: Move constructor and assignment operators don't need any
    //              code, the members will be reused automatically.
    PairingPQ(PairingPQ &&) noexcept = default;
    PairingPQ &operator=(PairingPQ &&) noexcept = default;


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        if (!root) return;

        std::deque<Node*> nodes;
        std::deque<Node*> q;
        q.push_back(root);

        while (!q.empty()) {
            Node* curr = q.front();
            q.pop_front();

            if (curr->child) q.push_back(curr->child);
            if (curr->sibling) q.push_back(curr->sibling);

            curr->child = nullptr;
            curr->sibling = nullptr;
            curr->previous = nullptr;
            nodes.push_back(curr);
        }

        root = nullptr;

        std::deque<Node*> pass;
        while (!nodes.empty()) {
            Node* a = nodes.front();
            nodes.pop_front();
            if (!nodes.empty()) {
                Node* b = nodes.front();
                nodes.pop_front();
                pass.push_back(meld(a, b));
            } else {
                pass.push_back(a);
            }
        }

        root = pass.back();
        pass.pop_back();

        while (!pass.empty()) {
            root = meld(root, pass.back());
            pass.pop_back();
        }
    }  // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) { addNode(val); }  // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        Node* p = root->child;

        --count;

        if (!p) {
            delete root;
            root = nullptr;
            return;
        }

        delete root;

        std::deque<Node*> pass;

        while (p) {
            Node* next = p->sibling;
            p->sibling = nullptr;
            p->previous = nullptr;

            if (next) {
                Node* nextNext = next->sibling;
                next->sibling = nullptr;
                next->previous = nullptr;

                // Meld the pair and push to queue
                pass.push_back(meld(p, next));
                p = nextNext;
            } else {
                pass.push_back(p);
                break;
            }
        }

        // Pass 2: Accumulate from back to front (or simply front-to-back logic)
        // Standard Pairing Heap accumulates result into the last tree
        root = pass.back();
        pass.pop_back();

        while (!pass.empty()) {
            root = meld(root, pass.back());
            pass.pop_back();
        }
    }  // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return **root;
    }  // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    [[nodiscard]] virtual std::size_t size() const {
        return count;
    }  // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    [[nodiscard]] virtual bool empty() const {
        return count == 0;
    }  // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node *node, const TYPE &new_value) {
        node->elt = new_value;

        if (node == root) return;

        if (node->previous->child == node) {
            node->previous->child = node->sibling;
        }
        else {
            node->previous->sibling = node->sibling;
        }

        if (node->sibling) {
            node->sibling->previous = node->previous;
        }

        node->sibling = nullptr;
        node->previous = nullptr;

        root = meld(root, node);
    }  // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node *addNode(const TYPE &val) {
        Node *new_node = new Node(val);
        root = meld(root, new_node);
        ++count;
        return new_node;
    }  // addNode()


private:
    Node *root = nullptr;
    std::size_t count = 0;

    Node* meld(Node *a, Node *b) {
        if (!a) return b;
        if (!b) return a;
        if (this->compare(**a, **b)) {
            a->sibling = b->child;
            if (b->child) b->child->previous = a;
            b->child = a;
            a->previous = b;
            return b;
        } else {
            b->sibling = a->child;
            if (a->child) a->child->previous = b;
            a->child = b;
            b->previous = a;
            return a;
        }
    }

    void destroyNode(Node* node) {
        while (node) {
            destroyNode(node->child);

            Node* next = node->sibling;
            delete node;
            node = next;
        }
    }

    Node* copyNode(Node* other) {
        if (!other) return nullptr;

        Node* firstNew = new Node(other->elt);

        Node* currentNew = firstNew;
        Node* currentOther = other;

        while (currentOther != nullptr) {
            if (currentOther->child) {
                currentNew->child = copyNode(currentOther->child);
                currentNew->child->previous = currentNew;
            }

            if (currentOther->sibling) {
                Node* nextNew = new Node(currentOther->sibling->elt);

                currentNew->sibling = nextNew;
                nextNew->previous = currentNew;

                currentNew = nextNew;
                currentOther = currentOther->sibling;
            } else {
                currentOther = nullptr;
            }
        }
        return firstNew;
    }

    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
};

#endif  // PAIRINGPQ_H
