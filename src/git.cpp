#include "git.h"
#include "../ext/git/git.h"


using namespace std;

string onest::git::getVersionInfo()
{
	if (!::git::IsPopulated())
		return "(unknown version)";

	string abbrevHash = ::git::CommitSHA1().substr(0, 8);
	string lastCommitMessage = ::git::CommitSubject();
	if (lastCommitMessage.size() > 50)
		lastCommitMessage = lastCommitMessage.substr(0, 47) + "...";

	string dirty = ::git::AnyUncommittedChanges() ? " (MODIFIED)" : "";

	return "Version: " + abbrevHash + " - " + lastCommitMessage + dirty;
}
