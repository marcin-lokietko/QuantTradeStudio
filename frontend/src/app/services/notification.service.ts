import { Injectable } from '@angular/core';
import { MatSnackBar } from '@angular/material/snack-bar';
import { NotificationComponent } from '../shared/components/notification/notification.component';
import { NotificationSeverity } from '../shared/components/notification/notification-severity-enum';

@Injectable({
  providedIn: 'root'
})
export class NotificationService {

  constructor(private snackBar: MatSnackBar) {}

  show(message: string, duration = 3000, severity = NotificationSeverity.Info) {
    this.snackBar.openFromComponent(NotificationComponent, {
      // 'data' is passed to NotificationComponent. The rest parametrizes MatSnackBar.
      data: {
        message,
        notificationSeverity : severity,
      },
      duration,
      horizontalPosition: 'center',
      verticalPosition: 'top',
      panelClass: ['common-notification']
    });
  }
}