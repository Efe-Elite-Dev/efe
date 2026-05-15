import tkinter as tk
from tkinter import messagebox
import os
import threading

class SkyStore(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Sky-Store")
        self.geometry("800x600")
        self.configure(bg='white')
        
        title_lbl = tk.Label(self, text="Sky-Store Uygulama Merkezi", font=("Helvetica", 24, "bold"), bg='white', fg='black')
        title_lbl.pack(pady=30)
        
        self.apps = [
            {"name": "Discord", "package": "discord"},
            {"name": "Spotify", "package": "spotify"},
            {"name": "Visual Studio Code", "package": "code --classic"},
            {"name": "Steam", "package": "steam"}
        ]
        
        self.create_app_list()

    def create_app_list(self):
        frame = tk.Frame(self, bg='white')
        frame.pack(fill='both', expand=True, padx=50)
        
        for app in self.apps:
            app_frame = tk.Frame(frame, bg='#f8f9fa', bd=1, relief='groove', pady=10)
            app_frame.pack(fill='x', pady=10)
            
            lbl = tk.Label(app_frame, text=app["name"], font=("Helvetica", 16), bg='#f8f9fa', fg='black')
            lbl.pack(side='left', padx=20)
            
            btn = tk.Button(app_frame, text="Yükle", font=("Helvetica", 12), bg='black', fg='white',
                            command=lambda a=app: self.start_install(a))
            btn.pack(side='right', padx=20)

    def start_install(self, app):
        threading.Thread(target=self.install_logic, args=(app,)).start()

    def install_logic(self, app):
        messagebox.showinfo("Sky-Store", f"{app['name']} Ubuntu altyapısı üzerinden kuruluyor...")
        exit_code = os.system(f"sudo snap install {app['package']}")
        
        if exit_code == 0:
            messagebox.showinfo("Başarılı", f"{app['name']} başarıyla sisteme entegre edildi!")
        else:
            messagebox.showerror("Hata", f"{app['name']} kurulurken bir sorun oluştu.")

if __name__ == "__main__":
    app = SkyStore()
    app.mainloop()
