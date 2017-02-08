import os

class TaskProjectBuilder:


    def __init__(self, taskDirectory, env, lib, bin2hex, Glob):
        self.taskDirectory = taskDirectory
        self.bin2hex = bin2hex
        self.env = env.Clone()
        self.Glob = Glob
        self.env.Append(CPPPATH=[taskDirectory + '/shared/include'])
        self.fullTaskLibrary = [ lib, self.env.Library([ file for file in self.Glob(self.taskDirectory + '/shared/src/*.c', True, True, True) ]) ]

    def buildSubTaskProject(self, subTaskName, subTaskSourceFile, includeSubTaskSpecificLib = False, taskSpecificLib = []):
        subtaskEnv = self.env.Clone()
        mainFile = self.Glob('/'.join([ self.taskDirectory, subTaskName, subTaskSourceFile]), True, True, True)[0]
        sources = [ mainFile ]
        if includeSubTaskSpecificLib:
            subtaskEnv.Append(CPPPATH=[ '/'.join([ self.taskDirectory, subTaskName ]) ])
            sources.extend([ '/'.join([ self.taskDirectory, subTaskName,  file]) for file in taskSpecificLib ])
        sources.extend(self.fullTaskLibrary);
        subtaskEnv.Program(sources)
        self.bin2hex(mainFile, subtaskEnv, 'esos')
        return self  # return self so that we can chain these calls.
