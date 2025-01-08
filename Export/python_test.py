from transformers import pipeline, AutoModelForCausalLM, AutoTokenizer
from enum import Enum
import json

class Hunk:
    def __init__(self, data: json):
        self.File_Name = data["file"]
        self.Old_Text_Start = data["old_start"]
        self.Old_Text_Line_Count = data["old_lines"]
        self.New_Text_Start = data["new_start"]
        self.New_Text_Line_Count = data["new_lines"]
        self.Old_Text = data["old_text"]
        self.New_Text = data["new_text"]

class Commit:
    def __init__(self, data: json):
        self.ID = data["id"]
        self.Message = data["message"]
        self.Hunks = [Hunk(hunk) for hunk in data["hunks"]]

class Node:
    def __init__(self, output: str, reference: Commit, input: str):
        self.Output = output
        self.Reference = reference
        self.Input = input

class AI:
    def __init__(self):
        self.Model = AutoModelForCausalLM.from_pretrained(model_path)
        self.Tokenizer = AutoTokenizer.from_pretrained(model_path)
        self.Pipe = pipeline(task="text-generation", model=Model, tokenizer=Tokenizer)
        self.History = {}

    def run(self, txt: str, reference: Commit):
        output = self.Pipe(txt, max_length=100, num_return_sequences=1, do_sample=True)[0]["generated_text"]
        node = Node(output, reference, txt)
        self.History[txt] = node

