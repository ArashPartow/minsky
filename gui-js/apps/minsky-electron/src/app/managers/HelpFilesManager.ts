import { promises as fsPromises } from 'fs';
import * as path from 'path';

abstract class HelpFilesManager {
  private static topicNodeMap: Record<string, string> = {};

  public static getHelpFileForType(type: string): string {
    if (type in this.topicNodeMap) {
      return this.topicNodeMap[type];
    }
    return null;
  }

  public static async initialize(directory: string) {
    await this.processFileOrDirectory(directory);
  }

  private static async processFileOrDirectory(fName: string) {
  console.log("processFileOrDirectory ",fName);
    let stat = null;
    try {
      stat = await fsPromises.lstat(fName);
    } catch (error) {
      console.warn(error);
    }

    if (!stat) {
      return;
    }

    if (stat.isFile()) {
      if (fName.endsWith('.html')) {
        await this.processDocumentFile(fName);
      }
    } else {
      const files = await fsPromises.readdir(fName);
      const promises = [];
      files.forEach(async (fileName) => {
        promises.push(this.processFileOrDirectory(fName + '/' + fileName));
      });
      await Promise.all(promises);
    }
  }

  private static async processDocumentFile(fName: string) {
    console.log("processDocumentFile ",fName);
    const buffer = await fsPromises.readFile(fName);
    if (buffer) {
      const contents = buffer.toString();
      const matches = contents.matchAll(/<A[ \t]+NAME="([^"]*)"/g);
      for (const match of matches) {
        console.log("Adding ",match[1],path.basename(fName));
        this.topicNodeMap[match[1]] = path.basename(fName);
      }
    }
  }
}

export { HelpFilesManager };
