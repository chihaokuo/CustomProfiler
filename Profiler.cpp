// Author: Chi-Hao Kuo
// Project: Custom Instrumented Profiler
// Updated: 11/11/2015

#include <fstream>
#include <iomanip>      // std::setprecision

#include "Profiler.h"

#pragma intrinsic(__rdtsc)

/*--------------------------------------------------------------------------*
Name:           PerfNode

Description:    Constructor.

Arguments:      name:	function name.

Returns:        None.
*---------------------------------------------------------------------------*/
PerfNode::PerfNode(const char *name)
	: parent_(nullptr), qpcfrequency_(0.0), qpcelapsedtime_(0.0), cyclestarttime_(0), cycleendtime_(0),
	cycleelapsedtime_(0), recursion_(0), calls_(1)
{
	function_name_.assign(name);
}

/*--------------------------------------------------------------------------*
Name:           ~PerfNode

Description:    Destructor.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
PerfNode::~PerfNode()
{
}

/*--------------------------------------------------------------------------*
Name:           TimerStart

Description:    Start the timer.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void PerfNode::TimerStart(void)
{
	// QueryPerformanceCounter
	{
		// get frequency
		QueryPerformanceFrequency(&qpcstarttime_);
		qpcfrequency_ = static_cast<double>(qpcstarttime_.QuadPart) / 1000.0;

		QueryPerformanceCounter(&qpcstarttime_);
	}

	// rdtsc
	{
		cyclestarttime_ = __rdtsc();
	}
}

/*--------------------------------------------------------------------------*
Name:           TimerEnd

Description:    End the timer.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void PerfNode::TimerEnd(void)
{
	// QueryPerformanceCounter
	{
		QueryPerformanceCounter(&qpcendtime_);
		qpcelapsedtime_ += (qpcendtime_.QuadPart - qpcstarttime_.QuadPart) / qpcfrequency_;
	}

	// rdtsc
	{
		cycleendtime_ = __rdtsc();
		cycleelapsedtime_ += cycleendtime_ - cyclestarttime_;
	}
}

/*--------------------------------------------------------------------------*
Name:           PrintTimer

Description:    Print out timer info.

Arguments:      level:		current level.
				file:		output file.

Returns:        None.
*---------------------------------------------------------------------------*/
void PerfNode::PrintTimer(unsigned int level, std::ofstream &file)
{
	for (unsigned int i = 0; i < level; ++i)
	{
		file << "\t";
		//printf_s("\t");
	}

	file << "\"" << function_name_ << "\"  ";
	file << "Calls: " << calls_;
	file << "  Time: " << std::setprecision(3) << NearZero(qpcelapsedtime_);
	file << "  Cycles: " << cycleelapsedtime_;
	if (parent_ == nullptr)
		file << "  %: " << std::setprecision(3) << 1.0 << std::endl;
	else
		file << "  %: " << std::setprecision(3) << NearZero(static_cast<double>(cycleelapsedtime_) / static_cast<double>(parent_->cycleelapsedtime_) * 100.0) << std::endl;
}

/*--------------------------------------------------------------------------*
Name:           PrintNode

Description:    Print out node.

Arguments:      level:		current level.
				file:		output file.

Returns:        None.
*---------------------------------------------------------------------------*/
void PerfNode::PrintNode(unsigned int level, std::ofstream &file)
{
	PrintTimer(level, file);

	for each (std::shared_ptr<PerfNode> node in children_)
	{
		node->PrintNode(level + 1, file);
	}
}

/*--------------------------------------------------------------------------*
Name:           IProfiler

Description:    Constructor.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
IProfiler::IProfiler()
	: level_(0)
{
	root_ = current_ = CreateNode("Root");
	nodelist_.push_back(root_);

	root_->TimerStart();
}

/*--------------------------------------------------------------------------*
Name:           IProfiler

Description:    Destructor.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
IProfiler::~IProfiler()
{
	root_->TimerEnd();

  #ifdef _RELEASE
	PrintTree("performance.log");
  #endif
}

/*--------------------------------------------------------------------------*
Name:           Enter

Description:    Enter a function.

Arguments:      name:		function name.

Returns:        None.
*---------------------------------------------------------------------------*/
void IProfiler::Enter(const char *name)
{
	++level_;

	std::shared_ptr<PerfNode> node = nullptr;

	if (strcmp(current_->function_name_.c_str(), name) == 0)
	{
		// recursive function

		++current_->recursion_;

		return;
	}
	else
	{
		// not recursive function
		// find if the child already exists, otherwise create child node

		node = FindChildren(name);

		if (node == nullptr)
		{
			node = CreateNode(name);
			node->parent_ = current_;
			current_->children_.push_back(node);
		}

		current_ = node;
	}

	node->TimerStart();
}

/*--------------------------------------------------------------------------*
Name:           Leave

Description:    Leave a function.

Arguments:      None.

Returns:        None.
*---------------------------------------------------------------------------*/
void IProfiler::Leave(void)
{
	--level_;

	if (current_->recursion_)
	{
		// in recursive iterations

		--current_->recursion_;

		return;
	}

	current_->TimerEnd();

	current_ = current_->parent_;
}

/*--------------------------------------------------------------------------*
Name:           PrintTree

Description:    Print out the entire tree.

Arguments:      filename:	filename to save.

Returns:        None.
*---------------------------------------------------------------------------*/
void IProfiler::PrintTree(const char * filename)
{
	std::ofstream treefile;

	// get mydocuments folder
	std::string mydoc;
	Util::GetFolderDirectory(mydoc, Util::FOLDER_ID::MY_DOCUMENTS);
	mydoc.append(filename);

	treefile.open(mydoc);

	root_->PrintNode(0, treefile);

	treefile.close();
}

/*--------------------------------------------------------------------------*
Name:           FindChildren

Description:    Find certain function from children.

Arguments:      name:						function name to find.

Returns:        std::shared_ptr<PerfNode>:	the node with function name.
				nullptr:					no child has the name.
*---------------------------------------------------------------------------*/
std::shared_ptr<PerfNode> IProfiler::FindChildren(const char *name)
{
	for each(std::shared_ptr<PerfNode> node in current_->children_)
	{
		if (strcmp(node->function_name_.c_str(), name) == 0)
		{
			++node->calls_;

			return node;
		}
	}

	return nullptr;
}

/*--------------------------------------------------------------------------*
Name:           CreateNode

Description:    Create a new node.

Arguments:      name:						function name to find.

Returns:        std::shared_ptr<PerfNode>:	the node with function name.
*---------------------------------------------------------------------------*/
std::shared_ptr<PerfNode> IProfiler::CreateNode(const char *name)
{
	std::shared_ptr<PerfNode> node = std::make_shared<PerfNode>(PerfNode(name));
	
	// add to nodelist for garbage collection
	nodelist_.push_back(node);

	return node;
}

/*--------------------------------------------------------------------------*
Name:			NearZero

Description:    Return 0 if number is too small.

Arguments:      number:		original number.

Returns:        double:		if abs(number) < 0.001 then return 0.
							otherwise return the number
*---------------------------------------------------------------------------*/
double NearZero(double number)
{
	if (abs(number) < 0.001)
		return 0.0;

	return number;
}
