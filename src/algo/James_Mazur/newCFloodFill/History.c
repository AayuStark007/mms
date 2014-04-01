#include <stdio.h>

#include "Cell.h"
#include "CellStack.h"
#include "History.h"
#include "List.h"

void h_initialize(struct History *hist, int stm, struct Cell *origin, bool firstTime) {

    // Set the short term memory
    hist->m_stm = stm;
    
    hist->m_size = 0;

    if (firstTime) {
        hist->m_stacks.front = NULL;
        hist->m_stacks.back = NULL;
        hist->m_checkpointStack = NULL;
        hist->m_stackReferenceCounts = newList();
        hist->m_modifiedCells = newList();
    } else {

        // Erase any left-over data
        while (!isEmptyMSQ(hist->m_stacks)) {
            popMSQ(hist->m_stacks);
        }
        while (!isEmpty(hist->m_stackReferenceCounts)) {
            pop_front(hist->m_stackReferenceCounts);
        }
        while (!isEmpty(hist->m_modifiedCells)) {
            struct List * temp = hist->m_modifiedCells->front;
            pop_front(hist->m_modifiedCells);
            destroyList(temp);
        }
        if (hist->m_checkpointStack != NULL) {
            destroyStack(hist->m_checkpointStack);
            hist->m_checkpointStack = NULL;
        }
    }

    // Push the stack [(0,0)] since (0,0) or is *always* our first target
    struct CellStack *temp = newStack();
    push(temp, origin);
    push(hist->m_stacks, temp);
    push_back(hist->m_stackReferenceCounts, 1);

    // Set the checkpoint values
    hist->m_checkpointCell = origin;
    hist->m_checkpointStack = temp;

}

struct Cell * getCheckpointCell(struct History *hist) {

    // The checkpoint cell is the prev cell for the cell on top of the stack for the 
    // frontmost (oldest) stack that we are storing within the hist->m_stacks queue.
    // Note that although we will have the same checkpoint cell if the top of
    // the stack contains either the origin (0,0) or a neighbor of the origin, but
    // these two situations are differentiated from one another by hist->m_checkpointStack
    // itself.
    
    return hist->m_checkpointCell;

}

struct CellStack * getCheckpointPath(struct History *hist) {

    // Returns a path from the origin to the checkpoint (exclusive) as a stack

    struct CellStack *path = newStack();
    struct Cell *runner = hist->m_checkpointCell;

    while (runner != NULL) {
        push(path,runner);
        runner = runner->m_prev; 
    }

    // The path always includes the origin, and since we can't and don't want 
    // to try to move to the origin, we can *always* pop it off safely
    pop(path);

    return path;

}

struct CellStack * getCheckpointStack(struct History *hist) {

    return copyOfStack(hist->m_checkpointStack);

}

void moved(struct History *hist) {

    hist->m_size++;

    // Assertion - Neither hist->m_stacks nor hist->m_stackReferenceCounts should ever be empty
    if (isEmpty(hist->m_stackReferenceCounts) || isEmptyMSQ(hist->m_stacks)) {
        printf("Error - History object has zero stack references\n");
        exit(0);
    }

    // Increment our stack reference counts
    int srefCount = back(hist->m_stackReferenceCounts);
    pop_back(hist->m_stackReferenceCounts);
    push_back(hist->m_stackReferenceCounts,srefCount+1);

    // Every move we push an empty list onto the list of lists of modified cells
    struct List * empty = newList(); // of Cellmod
    push_front(hist->m_modifiedCells, copyOfList(empty));

    // If the size of the path is larger than our short term memory (which it needn't
    // be) then reduce the reference counts and pop the appropriate number of things
    // off of the stacks.
    if (hist->m_size > hist->m_stm) {

        hist->m_size--;

        int srefCount2 = front(hist->m_stackReferenceCounts);
        pop_front(hist->m_stackReferenceCounts);
        push_front(hist->m_stackReferenceCounts,srefCount2-1);
    }

    if (front(hist->m_stackReferenceCounts) == 0) {

        pop_front(hist->m_stackReferenceCounts);
        pop(hist->m_stacks);
        destroyStack(hist->m_checkpointStack);
        hist->m_checkpointStack = front(hist->m_stacks);

        if (top(hist->m_checkpointStack)->m_prev != NULL) {
            hist->m_checkpointCell = top(hist->m_checkpointStack)->m_prev;
        } else {
            printf("Error - checkpoint has NULL prev Cell\n");
            exit(0);
        }

    }

    // Also make sure to only keep the correct number of modified cells
    if (size(hist->m_modifiedCells) > hist->m_stm) {
        struct List * temp = hist->m_modifiedCells->back;
        pop_back(hist->m_modifiedCells);
        destroyList(temp);
    }
}

void stackUpdate(struct History *hist, struct CellStack *newStack) { // JIMMY CHANGED PARAMETER TO POINTER

    // Assertion - Neither hist->m_stacks nor hist->m_stackReferenceCounts should ever be empty
    if (isEmpty(hist->m_stackReferenceCounts) || isEmptyMSQ(hist->m_stacks)) {
        printf("Error - History object has zero stack references\n");
        exit(0);
    }

    // When we perform the stack update, it's always after a cell has been pushed
    // to the path queue. However, in pushing the new Cell, we incremented the
    // reference count for that particular stack. Thus we need to decrement it
    // and then push the proper stack on hist->m_stacks, as well as a reference count
    // of 1 for the new stack on the stack reference count queue
    push(hist->m_stacks, copyOfStack(newStack));

    int temp = back(hist->m_stackReferenceCounts);
    pop_back(hist->m_stackReferenceCounts);
    push_back(hist->m_stackReferenceCounts,temp-1);
    push_back(hist->m_stackReferenceCounts,1);

}

void modifiedCellsUpdate(struct History *hist, struct List *cells) { // of Cellmod*

    // The only situation when this list will be empty is immediately after
    // returning to the origin after undo is called. This is because the checkpoint
    // stack will contain the origin, and thus won't move from the origin to get to
    // the target (the origin) but WILL perform the appropriate updates.
    if (size(hist->m_modifiedCells) > 0) {
        struct List * temp = hist->m_modifiedCells->front->data;
        pop_front(hist->m_modifiedCells);
        destroyList(temp,true);
    }
    push_front(hist->m_modifiedCells, copyOfList(cells,1));

}

void resetModifiedCells(struct History *hist) {

    struct List * temp = copyOfList(hist->m_modifiedCells); // of list of list of cellmod

    // Iterate through all modified cells, starting with most recent and going
    // to least recent. During iterations, we simply restore the old values
    while (!isEmpty(temp)) {

        struct List * cellList = copyOfList(front(temp)); // of Cellmod
        struct List * temp2 = front(temp); // list of Cellmod
        pop_front(temp);
        destroyList(temp2);

        struct ListNode * node = cellList->front;
        while (true) {
            node->data.cell->m_prev = node->data.oldPrev;
            node->data.cell->m_distance = node->data.oldDist;
            node->data.cell->m_explored = node->data.oldExplored;
            node->data.cell->m_traversed = node->data.oldTraversed;
            for (int i = 0; i < 4; i++) {
                node->data.cell->m_walls[i] = node->data.oldWalls[i];
                node->data.cell->m_wallsInspected[i] = node->data.oldWallsInspected[i];
            }
            if (node == cellList->back) {
                break;
            }
            node = node->behind;
        }

        destroyList(cellList);
    }

    // After we get a new checkpoint, we empty everything
    hist->m_size = 0;
    while (!isEmptyMSQ(hist->m_stacks)) {
        pop(hist->m_stacks);
    }
    while (!isEmpty(hist->m_stackReferenceCounts)) {
        pop_front(hist->m_stackReferenceCounts);
    }
    while (!isEmpty(hist->m_modifiedCells)) {
        struct List * temp = hist->m_modifiedCells->front;
        pop_front(hist->m_modifiedCells);
        destroyList(temp);
    }

    push(hist->m_stacks, hist->m_checkpointStack);
    push_back(hist->m_stackReferenceCounts, 1);
}