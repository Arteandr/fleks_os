#include "../includes/bm.h"
#include "../includes/os_status.h"
#include <cstdlib>

int BmCommand::execute(std::vector<std::string> args) {
  if (args.size() < 1 || args.size() > 2)
    return OS_SUCCESS;
  const std::string &arg = args[0];
  const std::string &group_str = args[1];
  const u8 group_no = std::atoi(group_str.c_str());
  bitmap *bm;

  if (arg == "block") {
    bm = this->fs.get_block_bitmap(group_no);
    bm->print();
  } else if (arg == "inode") {
    bm = this->fs.get_inode_bitmap(group_no);
    bm->print();
  } else if (arg == "table") {
    this->fs.print_inode_table(group_no);
  }

  delete bm;
  return OS_SUCCESS;
};
